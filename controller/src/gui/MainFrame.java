package gui;

import java.awt.CardLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JPanel;

import common.StringUtil;

import lime.LimeController;

public class MainFrame extends JFrame {
	private static final long serialVersionUID = -7114459738550431366L;

	private static final String CARD_CONNECT = "Connect Card";
	private static final String CARD_CONNECT2 = "Connect 2 Card";
	private static final String CARD_LOADING = "Loading Card";
	private static final String CARD_CONTROL = "Control Card";

	private LimeController control;

	private JPanel cardPanel;
	private ConnectPanel connectPanel;
	private Connect2Panel connect2Panel;
	private LoadingPanel loadingPanel;
	private ControlPanel controlPanel;
	private CardLayout cl;

	private MenuBar menuBar;
	
	private String validScripts[] = {"new", "test"};

	public MainFrame(LimeController control) {
		this.control = control;

		setTitle("key-lime-pi Controller");
		
		menuBar = new MenuBar(this);
		setJMenuBar(menuBar);

		cardPanel = new JPanel(new CardLayout());
		connectPanel = new ConnectPanel(control);
		connect2Panel = new Connect2Panel();
		loadingPanel = new LoadingPanel();
		controlPanel = new ControlPanel(this);
		cardPanel.add(connectPanel, CARD_CONNECT);
		cardPanel.add(connect2Panel, CARD_CONNECT2);
		cardPanel.add(loadingPanel, CARD_LOADING);
		cardPanel.add(controlPanel, CARD_CONTROL);

		add(cardPanel);
		cl = (CardLayout) (cardPanel.getLayout());
		cl.show(cardPanel, CARD_CONNECT);

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

	public void showConnect(String error) {
		connectPanel.setError(error);
		cl.show(cardPanel, CARD_CONNECT);
	}

	public void showConnect2(String error) {
		connect2Panel.setError(error);
		cl.show(cardPanel, CARD_CONNECT2);
	}

	public void showLoading(String msg) {
		loadingPanel.setLabel(msg);
		cl.show(cardPanel, CARD_LOADING);
	}

	public void showControl() {
		cl.show(cardPanel, CARD_CONTROL);
	}

	public void log(String str) {
		controlPanel.log(str);
	}

	public void resetControl() {
		controlPanel.reset();
	}
	
	public String[] getValidScripts(){
		return validScripts;
	}
	
	public void saveTitles(String path){
		controlPanel.saveTitles(path);
	}
	
	public void loadTitles(String path){
		controlPanel.loadTitles(path);
	}
	
	public boolean isScriptValid(String script) {
		return StringUtil.in_array(script, validScripts);
	}
}
