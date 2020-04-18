var express = require('express');
var router = express.Router();
const serialInterface = require('../serialInterface');

/* GET users listing. */
router.get('/debug', function(req, res, next) {
    return res.json({ success: true, data: serialInterface.getDebugLog(req.query.max) });
});
router.get('/card', function(req, res, next) {
    return res.json({ success: true, data: serialInterface.getCardLog(req.query.max) });
});

module.exports = router;
