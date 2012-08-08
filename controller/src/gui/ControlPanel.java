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

	//connection status panel
	private ConnectionPanel connection;
	// rss panel
	private RSSPanel rss;
	// title panel
	private TitlePanel title;
	// log panel
	private LogPanel log;

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

	public void log(String str) {
		log.log(str);
	}

	public void saveTitles(String path) {
		title.saveTitles(path);
	}

	public void loadTitles(String path) {
		title.loadTitles(path);
	}

	public void reset() {
		connection.reset();
		rss.reset();
		log.reset();
		title.reset();
	}
	
	public ConnectionPanel getConnectionPanel() {
		return connection;
	}
	
	public TitlePanel getTitlePanel() {
		return title;
	}
}
