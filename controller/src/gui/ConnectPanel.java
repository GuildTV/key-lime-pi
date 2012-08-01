package gui;

import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import lime.LimeController;

/**
 * Form to connect to master-pi
 * 
 * @author julus
 * 
 */
public class ConnectPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = -8052213006273646134L;
	
	private LimeController control;

	// form title
	private JLabel title;
	// text label
	private JLabel label;
	// input box
	private JTextField address;
	// submit button
	private JButton submit;
	// error label
	private JLabel error;

	ConnectPanel(LimeController control) {
		// define layout manager
		super(new GridBagLayout());
		
		this.control = control;
		
		// load constants
		GridBagConstraints c = new GridBagConstraints();
		c.ipady = 5;

		// create title
		title = new JLabel("Connect to master-pi");
		title.setFont(new Font("Dialog", 1, 20));
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = GridBagConstraints.REMAINDER;
		add(title, c);

		// create label
		label = new JLabel("Address:");
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth = 1;
		add(label, c);

		// create text field
		address = new JTextField(20);
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 1;
		c.gridy = 1;
		c.gridwidth = 1;
		add(address, c);

		// create submit button
		submit = new JButton("Connect");
		submit.addActionListener(this);
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 2;
		c.gridwidth = GridBagConstraints.REMAINDER;
		add(submit, c);

		// create label
		error = new JLabel();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 3;
		add(error, c);
	}

	/**
	 * Set the error message
	 * 
	 * @param str
	 *            Error message to show
	 */
	public void setError(String str) {
		error.setText(str);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
	    control.connect(address.getText());
	}
}
