package gui;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;

import common.StringUtil;

import lime.LimeController;

public class MainFrame extends JFrame {
	private static final long serialVersionUID = -7114459738550431366L;
	
	private LimeController control;

	private ControlPanel controlPanel;

	private MenuBar menuBar;

	private String validScripts[] = { "new", "test" };

	public MainFrame(LimeController control) {
		this.control = control;

		setTitle("key-lime-pi Controller");

		menuBar = new MenuBar(this);
		setJMenuBar(menuBar);

		controlPanel = new ControlPanel(this);

		add(controlPanel);

		setSize(800, 600);
		// pack();
		setVisible(true);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				System.exit(0);
			}
		});
	}

	public LimeController getControl() {
		return control;
	}

	public void log(String str) {
		controlPanel.log(str);
	}

	public void resetControl() {
		controlPanel.reset();
	}

	public String[] getValidScripts() {
		return validScripts;
	}

	public void saveTitles(String path) {
		controlPanel.saveTitles(path);
	}

	public void loadTitles(String path) {
		controlPanel.loadTitles(path);
	}

	public boolean isScriptValid(String script) {
		return StringUtil.in_array(script, validScripts);
	}

	public ControlPanel getControlPanel() {
		return controlPanel;
	}
}
