import express from 'express';
import type { Request, Response } from 'express';
import axios from 'axios';
import path from 'path';

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

const app = express();
const port = 3011;

// Replace with your ESP32's local IP address
const ARDUINO_URL = 'http://192.168.0.30:8077/send';

app.use(express.static(path.join(__dirname, '..', 'public')));
app.use(express.json());

app.post('/send-avr-vol-down', async (req: Request, res: Response) => {
  try {
    const response = await axios.post(ARDUINO_URL, {
      command: 'AVR_VOL_DOWN',
    });
    res.send(`Arduino responded: ${response.data}`);
  } catch (err) {
    console.error(err);
    res.status(500).send('Failed to contact Arduino');
  }
});

app.post('/ack', (req: Request, res: Response): void => {
  const { command, status } = req.body as { command?: string; status?: string };
  console.log(`Received ACK: Command=${command}, Status=${status}`);
  res.send('ACK received');
});

app.post('/trigger', (req: Request, res: Response): void => {
  const { command } = req.body as { command?: string };
  if (!command) {
    res.status(400).send("Missing 'command'");
    return;
  }

  res.send(`Triggered command: ${command}`);
});

app.listen(port, () => {
  console.log(`Web app running at http://localhost:${port}`);
});
