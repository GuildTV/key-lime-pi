package gui.control;

import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

public class LogPanel extends JPanel {
	private static final long serialVersionUID = -5018864002113251061L;

	private JTextArea textArea;
	private JScrollPane scrollPane;

	public LogPanel() {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Message Log"));

		textArea = new JTextArea(4, 20);
		scrollPane = new JScrollPane(textArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		textArea.setEditable(false);
		add(scrollPane, BorderLayout.CENTER);
	}

	public void log(String str) {
		textArea.insert(str + "\n", 0);
	}
	
	public void reset() {
		textArea.setText("");
	}
}
