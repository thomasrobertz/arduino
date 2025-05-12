"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const axios_1 = __importDefault(require("axios"));
const path_1 = __importDefault(require("path"));
/* Commands
AVR_ON_OFF
AVR_VOL_UP
AVR_VOL_DOWN
AVR_MUTE_UNMUTE
AVR_PC
AVR_PS5
TV_ON_OFF
TV_VOL_UP
TV_VOL_DOWN
TV_MUTE_UNMUTE
SCREEN_UP
SCREEN_DOWN
BEAMER_ON
BEAMER_OFF
*/
const app = (0, express_1.default)();
const port = 3011;
// Replace with your ESP32's local IP address
const ARDUINO_URL = 'http://192.168.0.30:8077/send';
app.use(express_1.default.static(path_1.default.join(__dirname, '..', 'public')));
app.use(express_1.default.json());
app.post('/send-avr-vol-down', async (req, res) => {
    try {
        const response = await axios_1.default.post(ARDUINO_URL, {
            command: 'AVR_VOL_DOWN',
        });
        res.send(`Arduino responded: ${response.data}`);
    }
    catch (err) {
        console.error(err);
        res.status(500).send('Failed to contact Arduino');
    }
});
app.post('/ack', (req, res) => {
    const { command, status } = req.body;
    console.log(`Received ACK: Command=${command}, Status=${status}`);
    res.send('ACK received');
});
app.post('/trigger', (req, res) => {
    const { command } = req.body;
    if (!command) {
        res.status(400).send("Missing 'command'");
        return;
    }
    res.send(`Triggered command: ${command}`);
});
app.listen(port, () => {
    console.log(`Web app running at http://localhost:${port}`);
});
