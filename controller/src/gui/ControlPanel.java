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
	
	// rss panel
	private RSSPanel rss;
	// title panel
	private TitlePanel title;
	// log panel
	private LogPanel log;

	ControlPanel(MainFrame owner) {
		super(new GridBagLayout());
		
		// create rss panel
		GridBagConstraints c1 = new GridBagConstraints();
		rss = new RSSPanel();
		c1.fill = GridBagConstraints.HORIZONTAL;
		c1.gridx = 0;
		c1.weightx = 1.0;
		c1.gridy = 0;
		add(rss, c1);

		// create log panel
		GridBagConstraints c2 = new GridBagConstraints();
		log = new LogPanel();
		c2.fill = GridBagConstraints.BOTH;
		c2.gridx = 0;
		c2.weightx = 1.0;
		c2.weighty = 0.2;
		c2.gridy = 1;
		add(log, c2);

		// create title panel
		GridBagConstraints c3 = new GridBagConstraints();
		title = new TitlePanel(owner);
		c3.fill = GridBagConstraints.BOTH;
		c3.gridx = 0;
		c3.weightx = 1.0;
		c3.weighty = 1.0;
		c3.gridy = 2;
		add(title, c3);
	}
	
	public void log(String str){
		log.log(str);
	}
	
	public void saveTitles(String path){
		title.saveTitles(path);
	}
	
	public void loadTitles(String path){
		title.loadTitles(path);
	}
	
	public void reset() {
		rss.reset();
		log.reset();
		title.reset();
	}
}
