"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
// Command queue implementation
class CommandQueue {
    constructor() {
        this.queue = [];
    }
    add(command) {
        this.queue.push(command);
    }
    getNextCommand() {
        return this.queue.shift();
    }
    clear() {
        this.queue = [];
    }
}
exports.default = CommandQueue;
