package gui;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import gui.control.*;

import javax.swing.JPanel;

/**
 * Create default control panel
 * 
 * @author julus
 * 
 */
public class ControlPanel extends JPanel {
	private static final long serialVersionUID = -2342321883822171371L;

	// connection status panel
	private ConnectionPanel connection;
	// rss panel
	private RSSPanel rss;
	// title panel
	private TitlePanel title;
	// log panel
	private LogPanel log;

	/**
	 * Create the control panel
	 * 
	 * @param owner
	 *            Frame parent
	 */
	ControlPanel(MainFrame owner) {
		super(new GridBagLayout());

		// create connection panel
		GridBagConstraints c = new GridBagConstraints();
		connection = new ConnectionPanel(owner);
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.weightx = 1.0;
		c.gridy = 0;
		add(connection, c);

		// create rss panel
		c = new GridBagConstraints();
		rss = new RSSPanel();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.weightx = 1.0;
		c.gridy = 1;
		add(rss, c);

		// create log panel
		c = new GridBagConstraints();
		log = new LogPanel();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.weightx = 1.0;
		c.weighty = 0.2;
		c.gridy = 2;
		add(log, c);

		// create title panel
		c = new GridBagConstraints();
		title = new TitlePanel(owner);
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.gridy = 3;
		add(title, c);
	}

	/**
	 * Write message to log
	 * 
	 * @param str
	 *            Message to log
	 */
	public void log(String str) {
		log.log(str);
	}

	/**
	 * Save titles to file
	 * 
	 * @param path
	 *            filename
	 */
	public void saveTitles(String path) {
		title.saveTitles(path);
	}

	/**
	 * Load titles from file
	 * 
	 * @param path
	 *            filename
	 */
	public void loadTitles(String path) {
		title.loadTitles(path);
	}

	/**
	 * Reset this control pane
	 */
	public void reset() {
		// reset connect
		connection.reset();
		// reset rss
		rss.reset();
		// reset log
		log.reset();
		// reset title
		title.reset();
	}

	/**
	 * Get the connection panel
	 * 
	 * @return Connection panel
	 */
	public ConnectionPanel getConnectionPanel() {
		return connection;
	}

	/**
	 * Get the title panel
	 * 
	 * @return Title panel
	 */
	public TitlePanel getTitlePanel() {
		return title;
	}
}
