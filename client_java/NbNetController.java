/**
 * This is a client for the FBIUHH network control patch for Neverball 1.5.4,
 * which allows the game to be controlled over a TCP socket. This is a
 * graphical client implemented in Java. It connects to Neverball and sends
 * values for x and z tilting that can be changed via the cursor keys.
 * 
 * (C) 2012 Julian Fietkau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import java.awt.BorderLayout;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.JFrame;
import javax.swing.JTextField;

@SuppressWarnings("serial")
public class NbNetController extends JFrame implements KeyListener {
	
	public short x, z, r, c;
	private Timer timer;
	private boolean[] keys = new boolean[525];
	private final String target_host = "localhost";

	public NbNetController() {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setSize(200, 200);

		final JTextField data = new JTextField();
		this.add(data, BorderLayout.NORTH);
		data.addKeyListener(this);
		data.setVisible(true);
		this.setVisible(true);

		Socket echoSocket = null;
		OutputStream out = null;

		try {
			echoSocket = new Socket(target_host, 33333);
			out = echoSocket.getOutputStream();
		} catch (UnknownHostException e) {
			System.err.println("Host is unknown: " + target_host);
			System.exit(1);
		} catch (IOException e) {
			System.err.println("Couldn't get I/O for "
					+ "the connection to: " + target_host);
			System.exit(1);
		}
		final OutputStream f_out = out;
		timer = new Timer();
		timer.schedule(new TimerTask() {
			public void run() {
				if (keys[KeyEvent.VK_UP]) {
					x = (short)Math.min(x+1000, 32000);
				}
				if (keys[KeyEvent.VK_DOWN]) {
					x = (short)Math.max(x-1000, -32000);
				}
				if (keys[KeyEvent.VK_LEFT]) {
					z = (short)Math.max(z-1000, -32000);
				}
				if (keys[KeyEvent.VK_RIGHT]) {
					z = (short)Math.min(z+1000, 32000);
				}
				// TODO: r is the camera rotation,
				//       c contains undocumented camera settings
				try {
					f_out.write(NbNetController.this.x);
					f_out.write(NbNetController.this.x >> 8);
					f_out.write(NbNetController.this.z);
					f_out.write(NbNetController.this.z >> 8);
					f_out.write(NbNetController.this.r);
					f_out.write(NbNetController.this.r >> 8);
					f_out.write(NbNetController.this.c);
					f_out.write(NbNetController.this.c >> 8);
					f_out.flush();
				} catch (IOException e) {
					// do nothing
				}
				
				data.setText("x="+NbNetController.this.x+", z="+NbNetController.this.z);
			}
		}, 0, 50);
	}

	public void keyPressed(KeyEvent e) {
		keys[e.getKeyCode()] = true;
	}

	public void keyReleased(KeyEvent e) {
		keys[e.getKeyCode()] = false;
	}

	public void keyTyped(KeyEvent e) {
	}

	public static void main(String[] args) {
		new NbNetController();
	}
}
