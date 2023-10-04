"use strict";
require("dotenv-safe").config();
const express = require("express");
const morgan = require("morgan");
const app = express();
const admin = require('firebase-admin');

const serviceAccount = require(process.env.GOOGLE_APPLICATION_CREDENTIAL);

const defaultApp = admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://monitoreosensores-cac35-default-rtdb.europe-west1.firebasedatabase.app/"});


const defaultDatabase = admin.database();

// app.use("/", (req, res) => {
//     res.json("Endpoint not here.")
// });

app.use("/data", (req, res) => {
    if (!req.headers.authorization || req.headers.authorization.indexOf('Basic ') === -1) {
        return res.status(401).json({ message: 'Missing Authorization Header' });
    }

    // verify auth credentials
    const base64Credentials = req.headers.authorization.split(' ')[1];
    const credentials = Buffer.from(base64Credentials, 'base64').toString('ascii');
    const [username, password] = credentials.split(':');
    const valid = username === process.env.SENSOR_USER && password === process.env.SENSOR_PASSWORD;
    if (!valid) {
        return res.status(401).json({ message: 'Invalid Authentication Credentials' });
    }

    let { t: temperature, h: humidity } = req.query;

    if (Number.isNaN(parseFloat(temperature ?? "")) || Number.isNaN(parseFloat(humidity ?? ""))) {
        return res.status(400).json({ message: "Temperature and humidity should be numbers" })
    } else {
        temperature = parseFloat(temperature);
        humidity = parseFloat(humidity);
        const now = Date.now();
        defaultDatabase.ref(`/datos/humedad/${now}`).set({ location: process.env.SERVER_LOCATION, value: humidity });
        defaultDatabase.ref(`/datos/temperatura/${now}`).set({ location: process.env.SERVER_LOCATION, value: temperature });

    }

    res.status(200).json("Endpoint not here.")
});

app.use(morgan("dev"));

const port = process.env.PORT ?? 3000;
app.listen(port, () => {
    console.log(`Listening on ${port}`)
})