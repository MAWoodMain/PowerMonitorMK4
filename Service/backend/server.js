const express = require("express");
const bodyParser = require("body-parser");
const logger = require("morgan");
const cors = require('cors');


const API_PORT = 3001;
const app = express();
const router = express.Router();

const cardRoute = require('./routes/CardRoute');
const dataRoute = require('./routes/DataRoute');
const logRoute = require('./routes/LogRoute');

app.use(cors());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
app.use(logger("dev"));

router.get("/", (req, res) => {
  res.json({ message: "HELLO WORLD!!!!" });
});

app.use('/api/cards', cardRoute);
app.use("/api/data", dataRoute);
app.use("/api/log", logRoute);


app.listen(API_PORT, () => console.log(`LISTENING ON PORT ${API_PORT}`));
