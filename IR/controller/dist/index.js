"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const axios_1 = __importDefault(require("axios"));
const path_1 = __importDefault(require("path"));
const Command_1 = require("./Command");
const CommandQueue_1 = __importDefault(require("./CommandQueue"));
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
const commandQueue = new CommandQueue_1.default();
// Process commands from the queue
async function processCommandQueue() {
    const command = commandQueue.getNextCommand();
    if (command) {
        switch (true) {
            case command instanceof Command_1.IRCommand:
                // Send IR command to Arduino
                try {
                    await axios_1.default.post(ARDUINO_URL, {
                        command: command.getName()
                    });
                    console.log(`Sent IR command: ${command.getName()}`);
                }
                catch (error) {
                    console.error(`Failed to send command: ${error}`);
                }
                break;
            case command instanceof Command_1.WaitCommand:
                // Handle wait command
                console.log(`Waiting for ${command.getMilliseconds()}ms`);
                await new Promise(resolve => setTimeout(resolve, command.getMilliseconds()));
                break;
            case command instanceof Command_1.RepeatCommand:
                // Handle repeat command - it will reschedule itself if needed
                try {
                    const irRepeatCommand = command.getCommand();
                    await axios_1.default.post(ARDUINO_URL, {
                        command: irRepeatCommand.getName()
                    });
                    console.log(`Sent repeated IR command: ${irRepeatCommand.getName()}`);
                    // The command will handle rescheduling itself
                    command.execute();
                }
                catch (error) {
                    console.error(`Failed to send repeated command: ${error}`);
                }
                break;
            default:
                console.warn(`Unknown command type: ${command.constructor.name}`);
                break;
        }
    }
    // Check again after a short delay
    setTimeout(processCommandQueue, 100);
}
// Start processing the command queue
processCommandQueue();
app.use(express_1.default.static(path_1.default.join(__dirname, '..', 'public')));
app.use(express_1.default.json());
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
    // Check if it's a wait command (format: "WAIT:1000" for 1000ms)
    if (command.startsWith('WAIT:')) {
        const ms = parseInt(command.split(':')[1], 10);
        if (isNaN(ms)) {
            res.status(400).send("Invalid wait format. Use 'WAIT:milliseconds'");
            return;
        }
        commandQueue.add(new Command_1.WaitCommand(ms));
        res.send(`Wait for ${ms}ms added to queue`);
    }
    // Check if it's a repeat command (format: "REPEAT:command:interval:count")
    else if (command.startsWith('REPEAT:')) {
        const parts = command.split(':');
        if (parts.length !== 4) {
            res.status(400).send("Invalid repeat format. Use 'REPEAT:command:interval:count'");
            return;
        }
        const cmdName = parts[1];
        const interval = parseInt(parts[2], 10);
        const count = parseInt(parts[3], 10);
        if (isNaN(interval) || isNaN(count)) {
            res.status(400).send("Invalid interval or count in repeat command");
            return;
        }
        const irCommand = new Command_1.IRCommand(cmdName);
        const repeatCommand = new Command_1.RepeatCommand(irCommand, interval, count, commandQueue);
        commandQueue.add(repeatCommand);
        res.send(`Added repeat command: ${cmdName} (${count} times, every ${interval}ms)`);
    }
    // Regular IR command
    else {
        commandQueue.add(new Command_1.IRCommand(command));
        res.send(`Triggered command: ${command}`);
    }
});
app.post('/stop', (req, res) => {
    // Clear the command queue
    while (commandQueue.getNextCommand()) {
        // Just drain the queue
    }
    console.log('Command queue cleared');
    res.send('All commands stopped');
});
app.listen(port, () => {
    console.log(`Web app running at http://localhost:${port}`);
});
