import { Command, IRCommand, WaitCommand } from './Command';

// Command queue implementation
export default class CommandQueue {
  private queue: Command[] = [];

  add(command: Command): void {
    this.queue.push(command);
  }

  getNextCommand(): Command | undefined {
    return this.queue.shift();
  }
  
  clear(): void {
    this.queue = [];
  }
}
