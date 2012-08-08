package net;
/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

/**
 * Simple class to hold a message recieved from the network
 * 
 * @author julus
 * 
 */
public class NetMessage {
	//length of message
	private int messageLength;
	//message body
	private String message;
	//whether the message is complete
	private boolean complete;

	NetMessage(int length, String msg){
		messageLength = length;
		message = msg;
		complete = false;
	}
		
	/**
	 * Get the length of the message
	 * @return length
	 */
	public int getMessageLength() {
		return messageLength;
	}

	/**
	 * Get the message
	 * @return message
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * Append to the end of the message, if not complete
	 * @param msg Content to add
	 */
	public void appendMessage(String msg) {
		if (!complete)
			message += msg;
	}

	/**
	 * Check if this message is complete
	 * @return complete
	 */
	public boolean isComplete() {
		return complete;
	}

	/**
	 * Set this message as completed
	 */
	public void setComplete() {
		complete = true;
	}

	@Override
	public String toString() {
		return "NetMessage [message=" + message + ", length=" + messageLength + ", complete=" + complete + "]";
	}
}
