package gui.control;

import gui.MainFrame;

import java.awt.BorderLayout;
import java.awt.Graphics;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import common.IOUtil;

import JSON.JSONArray;
import JSON.JSONException;
import JSON.JSONObject;
import JSON.JSONStringer;

/**
 * Panel containing the all of the TitleElement
 * 
 * @author julus
 * 
 */
public class TitlePanel extends JPanel {
	private static final long serialVersionUID = 8021157860217699741L;

	// amount to increase storage array by
	private static final int ARRAY_INCREMENT = 10;

	// frame owner
	private MainFrame owner;

	// lock for array
	private Object arrayLock = new Object();

	// scroll pane
	private JScrollPane pane;
	// panel inside scroll pane
	private JPanel panel;

	// panel of buttons
	private TitleButtonPanel buttonPanel;

	// array of elements
	private TitleElement elements[];
	// element count
	private int elmCount = 0;

	// element that is selected
	private TitleElement selected;
	// element that has been preloaded
	private TitleElement loaded;

	/**
	 * Create the TitlePanel
	 * 
	 * @param owner
	 *            Frame owner
	 */
	public TitlePanel(MainFrame owner) {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Title Control"));

		this.owner = owner;

		// create and add the button panel
		buttonPanel = new TitleButtonPanel(this);
		add(buttonPanel, BorderLayout.NORTH);

		// create and add the scroll pane
		panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		pane = new JScrollPane(panel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		pane.setBorder(BorderFactory.createEmptyBorder());
		add(pane, BorderLayout.CENTER);

		// create new element storages
		elements = new TitleElement[ARRAY_INCREMENT];
		loaded = null;
	}

	/**
	 * Create a new element
	 * 
	 * @return New element
	 */
	public TitleElement create() {
		// create it
		TitleElement elm = new TitleElement(this);
		// add to array
		addElm(elm);
		// return it
		return elm;
	}

	/**
	 * Add the element to array
	 * 
	 * @param elm
	 *            Element to add
	 */
	public void addElm(TitleElement elm) {
		// get lock
		synchronized (arrayLock) {
			try {
				// try adding and increase count
				elements[elmCount] = elm;
				elmCount++;
			} catch (ArrayIndexOutOfBoundsException aoe) {
				// create a bigger array
				TitleElement elms[] = new TitleElement[elements.length + ARRAY_INCREMENT];
				// copy data into it
				System.arraycopy(elements, 0, elms, 0, elmCount);
				// add new item
				elms[elmCount] = elm;
				// increate count
				elmCount++;
				// overwrite old array
				elements = elms;
			}
		}

		// add element to panel
		panel.add(elm);
	}

	/**
	 * Remove the element from the array
	 * 
	 * @param elm
	 */
	public void removeElm(TitleElement elm) {
		// get array lock
		synchronized (arrayLock) {
			// remove element with position in array
			removeElm(getPosElm(elm));
		}
	}

	/**
	 * Get the position of an element in the array
	 * 
	 * @param elm
	 *            Element to find
	 * @return Position of the element
	 */
	public int getPosElm(TitleElement elm) {
		// get array lock
		synchronized (arrayLock) {
			// search for element
			int i;
			for (i = 0; i < elmCount; i++) {
				if (elements[i] == elm)
					break;
			}

			// return error, if not found
			if (i >= elmCount)
				return -1;

			// return position
			return i;
		}
	}

	/**
	 * Move element up in the list/array
	 * 
	 * @param elm
	 */
	public void moveUpElm(TitleElement elm) {
		synchronized (arrayLock) {
			// get the position of the element
			int pos = getPosElm(elm);
			// if not found, fail
			if (pos <= 0)
				return;

			// swap the elements
			elements[pos] = elements[pos - 1];
			elements[pos - 1] = elm;
		}
	}

	/**
	 * Move element down in the list/array
	 * 
	 * @param elm
	 */
	public void moveDownElm(TitleElement elm) {
		synchronized (arrayLock) {
			// get the position of the element
			int pos = getPosElm(elm);
			// if not foung, or exceeds array, fail
			if (pos < 0 || pos == elmCount - 1)
				return;

			// swap the elements
			elements[pos] = elements[pos + 1];
			elements[pos + 1] = elm;
		}
	}

	/**
	 * Remove an element from the array
	 * 
	 * @param i
	 *            Index of the element
	 */
	public void removeElm(int i) {
		// fail if index is invalid
		if (i > elmCount || i < 0)
			return;

		// remove element from the panel
		panel.remove(elements[i]);

		// get the array lock
		synchronized (arrayLock) {
			// create a new array
			TitleElement elms[] = new TitleElement[elements.length];
			// copy data across
			System.arraycopy(elements, 0, elms, 0, i);
			System.arraycopy(elements, i, elms, i + 1, elements.length - i - 1);
			// save and decrease count
			elements = elms;
			elmCount--;
		}
	}

	/**
	 * Remove and readd all the elements
	 */
	public void redrawElms() {
		synchronized (arrayLock) {
			// remove them all
			panel.removeAll();
			for (int i = 0; i < elmCount; i++) {
				// add them all
				panel.add(elements[i]);
			}
		}
	}

	/**
	 * Get the frame
	 * 
	 * @return Frame
	 */
	public MainFrame getOwner() {
		return owner;
	}

	/**
	 * Reset the panel
	 */
	public void reset() {
		// create new array
		elements = new TitleElement[ARRAY_INCREMENT];
		// remove all the elements
		panel.removeAll();
		// set nothing as selected and loaded
		selected = null;
		loaded = null;
	}

	/**
	 * Set the currently selected element
	 * 
	 * @param elm
	 *            Element that has been selected
	 */
	public void setSelectedElement(TitleElement elm) {
		selected = elm;
		// enable/disable button panel
		if (elm == null)
			buttonPanel.setNull();
		else
			buttonPanel.setElm();
	}

	/**
	 * Get the currently selected element
	 * 
	 * @return Selected Element
	 */
	public TitleElement getSelectedElement() {
		return selected;
	}

	/**
	 * Save the titles to file
	 * 
	 * @param path
	 *            Filename
	 */
	public void saveTitles(String path) {
		// create json stringer
		JSONStringer stringer = new JSONStringer();
		try {
			// start off json
			stringer.object();
			stringer.key("data").array();

			synchronized (arrayLock) {
				// for each element
				for (int i = 0; i < elmCount; i++) {
					try {
						// convert to json
						elements[i].toJSON(stringer);
					} catch (JSONException e) {
						// log error
						getOwner().log("Failed to save a title element to file");
					}
				}
			}

			// close up
			stringer.endArray();
			stringer.endObject();

			// write to file, or log error
			if (!IOUtil.writeFile(path, stringer.toString()))
				getOwner().log("Failed to write to output file");

		} catch (JSONException e) {
			// log error
			getOwner().log("Failed to save titles to file");
		}
	}

	public void loadTitles(String path) {
		// reset all panels
		reset();

		try {
			// read json from file
			JSONObject json = new JSONObject(IOUtil.readFile(path));
			JSONArray data = json.getJSONArray("data");

			// for each element
			for (int i = 0; i < data.length(); i++) {
				try {
					// create from json
					JSONObject o = data.getJSONObject(i);
					addElm(new TitleElement(this, o));
				} catch (JSONException e) {
					// log error
					getOwner().log("Failed to load a title from file");
				}
			}

		} catch (JSONException e) {
			// log error
			getOwner().log("Failed to load save");
		}

		// force repaint
		revalidate();
		repaint();
	}

	/**
	 * Modify painting
	 */
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		// hide button panel, if in live mode
		if (owner.getControl().isLive())
			buttonPanel.setVisible(false);
		else
			buttonPanel.setVisible(true);

	}

	/**
	 * Set an element as preloaded one
	 * 
	 * @param elm
	 *            Element to mark as preloaded
	 */
	public void setLoaded(TitleElement elm) {
		loaded = elm;
		repaint();
	}

	/**
	 * Get the element that is preloaded
	 * 
	 * @return Preloaded element
	 */
	public TitleElement getLoaded() {
		return loaded;
	}

	/**
	 * Set an element as preloaded by name
	 * 
	 * @param name
	 *            String name
	 */
	public void setLoadedByName(String name) {
		synchronized (arrayLock) {
			// check each element
			for (int i = 0; i < elmCount; i++) {
				TitleElement elm = elements[i];
				if (elm.getNameValue().equals(name)) {
					// set it as loaded
					setLoaded(elm);
				}
			}
		}
	}

	/**
	 * Count the elements with specified name
	 * 
	 * @param name
	 *            Name to match
	 * @return Count of matching elements
	 */
	public int countElmName(String name) {
		// start off count
		int count = 0;
		synchronized (arrayLock) {
			// check each element
			for (int i = 0; i < elmCount; i++) {
				TitleElement elm = elements[i];
				if (elm.getNameValue().equals(name)) {
					// increase count
					count++;
				}
			}
		}
		// return matched count
		return count;
	}
}
