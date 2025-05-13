export abstract class Command {
    abstract execute(): void;
}

export class IRCommand extends Command {
    private name: string;

    constructor(name: string) {
        super();
        this.name = name;
    }

    execute(): void {
        console.log(`Executing IR command: ${this.name}`);
        // The actual HTTP implementation will be handled elsewhere
    }
    
    getName(): string {
        return this.name;
    }
}

export class WaitCommand extends Command {
    private milliseconds: number;

    constructor(milliseconds: number) {
        super();
        this.milliseconds = milliseconds;
    }

    execute(): void {
        console.log(`Waiting for ${this.milliseconds}ms`);
        // The actual waiting implementation will be handled elsewhere
    }
    
    getMilliseconds(): number {
        return this.milliseconds;
    }
}

export class RepeatCommand extends Command {
    private command: IRCommand;
    private interval: number;
    private remainingRepeats: number;
    private infiniteRepeat: boolean;
    private queue: any; // Reference to the command queue

    constructor(command: IRCommand, interval: number, repeats: number, queue: any) {
        super();
        this.command = command;
        this.interval = interval;
        this.infiniteRepeat = (repeats === 0);
        this.remainingRepeats = this.infiniteRepeat ? 1 : repeats; // If infinite, just use 1 as placeholder
        this.queue = queue;
    }

    execute(): void {
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
            } else {
                console.log(`Repeating command ${this.command.getName()} ${this.remainingRepeats} more times`);
            }
            
            // Add a wait command for the interval
            this.queue.add(new WaitCommand(this.interval));
            
            // Add this command back to the queue
            this.queue.add(this);
        } else {
            console.log(`Finished repeating command ${this.command.getName()}`);
        }
    }
    
    getCommand(): IRCommand {
        return this.command;
    }
    
    getInterval(): number {
        return this.interval;
    }
    
    getRemainingRepeats(): number {
        return this.remainingRepeats;
    }
    
    isInfinite(): boolean {
        return this.infiniteRepeat;
    }
}
