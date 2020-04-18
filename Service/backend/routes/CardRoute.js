var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
    return res.json({ success: true, data: [
            {state: "RUNNING", type: "VC-01", channels: [{name: "Voltage"}, {name: "Current"}]},
            {state: "CONFIGURED", type: "CCC-01", channels: [{name: "Current"}, {name: "Current"}, {name: "Current"}]},
            {},
            {state: "UNIDENTIFIED"}] });
});

module.exports = router;
