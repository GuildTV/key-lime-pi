package gui.control;
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
 * Store a data value pair
 * 
 * @author julus
 * 
 */
public class TitleData {

	// name
	private String name = "";
	// value
	private String value = "";

	/**
	 * Create empty data pair
	 */
	public TitleData() {
	}

	/**
	 * Create data pair
	 * 
	 * @param name
	 *            Starting name
	 * @param value
	 *            Starting value
	 */
	public TitleData(String name, String value) {
		this.name = name;
		this.value = value;
	}

	/**
	 * Get the name
	 * 
	 * @return String name
	 */
	public String getName() {
		return name;
	}

	/**
	 * Set the name
	 * 
	 * @param name
	 *            String name
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * Get the value
	 * 
	 * @return String value
	 */
	public String getValue() {
		return value;
	}

	/**
	 * Set the value
	 * 
	 * @param value
	 *            String value
	 */
	public void setValue(String value) {
		this.value = value;
	}

	/**
	 * Convert this to string
	 */
	@Override
	public String toString() {
		return name + ": " + value;
	}
}
