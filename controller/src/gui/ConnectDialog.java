package gui;

import gui.control.Connect;

import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

/**
 * Dialog to connect to master/slave
 * 
 * @author julus
 * 
 */
public class ConnectDialog extends JDialog implements ActionListener {
	private static final long serialVersionUID = 5623899832730572977L;

	// frame owner
	private MainFrame owner;
	// destination of conenct
	private Connect destination;

	// form title
	private JLabel title;
	// text label
	private JLabel label;
	// input box
	private JTextField address;
	// submit button
	private JButton submit;
	// cancel button
	private JButton cancel;
	// error label
	private JLabel error;

	/**
	 * Create dialog
	 * 
	 * @param owner
	 *            Frame owner
	 * @param dest
	 *            Destination
	 */
	public ConnectDialog(MainFrame owner, Connect dest) {
		this(owner, dest, "");
	}

	/**
	 * Create dialog
	 * 
	 * @param owner
	 *            Frame owner
	 * @param dest
	 *            Destination
	 * @param msg
	 *            Error message
	 */
	public ConnectDialog(MainFrame owner, Connect dest, String msg) {
		super(owner);

		this.owner = owner;
		destination = dest;

		// determine name from destination
		String name = "";
		if (destination == Connect.SLAVE)
			name = "slave-pi";
		else if (destination == Connect.MASTER)
			name = "master-pi";

		// set title
		setTitle("Connect to " + name);

		// setup dialog frame
		setSize(500, 200);
		setModal(true);
		setResizable(false);
		setLayout(new GridBagLayout());

		// load constants
		GridBagConstraints c = new GridBagConstraints();
		c.ipady = 5;

		// create title
		title = new JLabel("Connect to " + name);
		title.setFont(new Font("Dialog", 1, 20));
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 2;
		add(title, c);

		// create label
		label = new JLabel("Address:");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth = 1;
		add(label, c);

		// create text field
		address = new JTextField(20);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 1;
		c.gridy = 1;
		add(address, c);
		address.setText("127.0.0.1");// TODO remove later

		JPanel bPan = new JPanel();
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 2;
		c.gridwidth = 2;
		add(bPan, c);

		// create submit button
		submit = new JButton("Connect");
		submit.addActionListener(this);
		submit.setActionCommand("submit");
		bPan.add(submit);

		// create submit button
		cancel = new JButton("Cancel");
		cancel.addActionListener(this);
		cancel.setActionCommand("cancel");
		bPan.add(cancel);

		// create label
		error = new JLabel(msg);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 3;
		c.gridwidth = 2;
		add(error, c);
	}

	/**
	 * Handle actions
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("submit")) {
			// handle connect to master
			if (destination == Connect.MASTER) {
				// connect to address
				String ret = owner.getControl().connect(address.getText());
				if (ret == null)
					// close on success
					dispose();
				else
					// set error on failure
					error.setText(ret);
			} else {
				// TODO - handle slave connection
			}
		} else if (e.getActionCommand().equals("cancel")) {
			// close on cancel button
			dispose();
		}
	}
}
