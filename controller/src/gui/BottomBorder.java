package gui;
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

import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Insets;

import javax.swing.border.AbstractBorder;

/**
 * Custom border only on bottom of a panel
 * 
 * @author julus
 * 
 */
public class BottomBorder extends AbstractBorder {
	private static final long serialVersionUID = -6059320628247720179L;

	protected int thickness;

	protected Color lineColor;

	protected int gap;

	public BottomBorder(Color color) {
		this(color, 1, 1);
	}

	public BottomBorder(Color color, int thickness) {
		this(color, thickness, thickness);
	}

	public BottomBorder(Color color, int thickness, int gap) {
		lineColor = color;
		this.thickness = thickness;
		this.gap = gap;
	}

	public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
		Color oldColor = g.getColor();
		int i;

		g.setColor(lineColor);
		for (i = 0; i < thickness; i++) {
			g.drawLine(x, y + height - i - 1, x + width, y + height - i - 1);
		}
		g.setColor(oldColor);
	}

	public Insets getBorderInsets(Component c) {
		return new Insets(0, 0, gap, 0);
	}

	public Insets getBorderInsets(Component c, Insets insets) {
		insets.left = 0;
		insets.top = 0;
		insets.right = 0;
		insets.bottom = gap;
		return insets;
	}

	/**
	 * Returns the color of the border.
	 */
	public Color getLineColor() {
		return lineColor;
	}

	/**
	 * Returns the thickness of the border.
	 */
	public int getThickness() {
		return thickness;
	}

	/**
	 * Returns whether or not the border is opaque.
	 */
	public boolean isBorderOpaque() {
		return false;
	}

	public int getGap() {
		return gap;
	}

}