var express = require('express');
var router = express.Router();
const ArrayList = require('arraylist');

const localStore = new ArrayList;

router.get("/getData", (req, res) => {
    return res.json({ success: true, data: localStore.toArray() });
});

router.post("/updateData", (req, res) => {
    const { id, message } = req.body;
    localStore.set(id, {message: message, id: id});
    return res.json({ success: true });
});

router.delete("/deleteData", (req, res) => {
    const { id } = req.body;
    localStore.remove(id);
    return res.json({ success: true });
});

router.post("/putData", (req, res) => {
    const { id, message } = req.body;
    localStore.set(id, {message: message, id: id});

    if ((!id && id !== 0) || !message) {
        return res.json({
            success: false,
            error: "INVALID INPUTS"
        });
    }
    return res.json({ success: true });
});

module.exports = router;