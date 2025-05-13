"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.RepeatCommand = exports.WaitCommand = exports.IRCommand = exports.Command = void 0;
class Command {
}
exports.Command = Command;
class IRCommand extends Command {
    constructor(name) {
        super();
        this.name = name;
    }
    execute() {
        console.log(`Executing IR command: ${this.name}`);
        // The actual HTTP implementation will be handled elsewhere
    }
    getName() {
        return this.name;
    }
}
exports.IRCommand = IRCommand;
class WaitCommand extends Command {
    constructor(milliseconds) {
        super();
        this.milliseconds = milliseconds;
    }
    execute() {
        console.log(`Waiting for ${this.milliseconds}ms`);
        // The actual waiting implementation will be handled elsewhere
    }
    getMilliseconds() {
        return this.milliseconds;
    }
}
exports.WaitCommand = WaitCommand;
class RepeatCommand extends Command {
    constructor(command, interval, repeats, queue) {
        super();
        this.command = command;
        this.interval = interval;
        this.infiniteRepeat = (repeats === 0);
        this.remainingRepeats = this.infiniteRepeat ? 1 : repeats; // If infinite, just use 1 as placeholder
        this.queue = queue;
    }
    execute() {
        // Execute the wrapped command
        this.command.execute();
        // Only decrement if not infinite
        if (!this.infiniteRepeat) {
            this.remainingRepeats--;
        }
        // If infinite or we have repeats left, add a wait and then reschedule this command
        if (this.infiniteRepeat || this.remainingRepeats > 0) {
            if (this.infiniteRepeat) {
                console.log(`Repeating command ${this.command.getName()} infinitely`);
            }
            else {
                console.log(`Repeating command ${this.command.getName()} ${this.remainingRepeats} more times`);
            }
            // Add a wait command for the interval
            this.queue.add(new WaitCommand(this.interval));
            // Add this command back to the queue
            this.queue.add(this);
        }
        else {
            console.log(`Finished repeating command ${this.command.getName()}`);
        }
    }
    getCommand() {
        return this.command;
    }
    getInterval() {
        return this.interval;
    }
    getRemainingRepeats() {
        return this.remainingRepeats;
    }
    isInfinite() {
        return this.infiniteRepeat;
    }
}
exports.RepeatCommand = RepeatCommand;
