package gui.control;

import gui.MainFrame;

import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;

import common.StringUtil;

/**
 * Dialog box to edit
 * 
 * @author julus
 * 
 */
public class TitleElementEditDialog extends JDialog implements ActionListener {
	private static final long serialVersionUID = 8900911924444729087L;

	// frame owner
	private MainFrame owner;
	// element being edited
	private TitleElement element;
	// name edit box
	private JTextField nameField;
	// script name selector
	private JComboBox<String> scriptNameField;
	// data table
	private JTable dataTable;
	// table scroll pane
	private JScrollPane tablePanel;

	// table model
	private TitleElementTableModel tableModel;

	/**
	 * Create edit dialog
	 * 
	 * @param owner
	 *            Frame owner
	 * @param element
	 *            Element being edited
	 */
	public TitleElementEditDialog(MainFrame owner, final TitleElement element) {
		super(owner);
		this.element = element;
		this.owner = owner;

		// set dialog basics
		setTitle("Edit Selected Title");
		setModal(true);
		setSize(500, 250);
		setResizable(false);
		JPanel panel = new JPanel(new GridBagLayout());
		add(panel);

		// add title
		JLabel title = new JLabel("Edit Title Element");
		title.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 4;
		c.insets = new Insets(5, 5, 5, 5);
		panel.add(title, c);

		// name label
		JLabel label = new JLabel("Name: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(label, c);

		// name field
		nameField = new JTextField(element.getNameValue(), 20);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1.0;
		c.gridwidth = 2;
		c.gridx = 1;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(nameField, c);

		// name save button
		JButton save = new JButton("Save Name");
		save.addActionListener(this);
		save.setActionCommand("Set Name");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 3;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(save, c);

		// script label
		JLabel label2 = new JLabel("Script/Video: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 2;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(label2, c);

		// script selector
		scriptNameField = new JComboBox<String>(owner.getValidScripts());
		scriptNameField.addActionListener(this);
		scriptNameField.setActionCommand("Set Script");
		int index = StringUtil.array_search(element.getScriptName(), owner.getValidScripts());
		if (index != -1)
			scriptNameField.setSelectedIndex(index);
		else
			scriptNameField.setSelectedIndex(0);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 1;
		c.gridy = 2;
		c.gridwidth = 3;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(scriptNameField, c);

		// data table
		tableModel = new TitleElementTableModel(element);
		dataTable = new JTable(tableModel);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 3;
		c.gridwidth = 4;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.insets = new Insets(0, 5, 5, 5);
		tablePanel = new JScrollPane(dataTable, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		panel.add(tablePanel, c);

		// table row add button
		JButton addRow = new JButton("Add Row");
		addRow.addActionListener(this);
		addRow.setActionCommand("Add Row");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 0;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(addRow, c);

		// table row delete button
		JButton delRow = new JButton("Delete Row");
		delRow.addActionListener(this);
		delRow.setActionCommand("Delete Row");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 1;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(delRow, c);

		// delete element button
		JButton delete = new JButton("Delete");
		delete.addActionListener(this);
		delete.setActionCommand("Delete");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 2;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(delete, c);

		// close dialog button
		JButton close = new JButton("Close");
		close.addActionListener(this);
		close.setActionCommand("Close");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 3;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(close, c);

		// add closing listener
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent event) {
				dispose();
			}
		});
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();
		// set the name
		if (action.equals("Set Name")) {
			// check name isnt already in use
			int count = element.getMyParent().countElmName(nameField.getText());
			if (count == 0 || (count == 1 && element.getNameValue().equals(nameField.getText())))
				// save name
				element.setNameValue(nameField.getText());

			// set the script
		} else if (action.equals("Set Script")) {
			element.setScriptName((String) scriptNameField.getSelectedItem());

			// close the window
		} else if (action.equals("Close"))
			dispose();

		// delete the element
		else if (action.equals("Delete")) {
			// confirmation dialog
			Object[] options = { "Yes", "No" };
			int n = JOptionPane.showOptionDialog(owner, "Are you sure you want to delete this?", "Confirm Deletion",
					JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[1]);
			if (n == JOptionPane.YES_OPTION) {
				// delete element
				element.getMyParent().removeElm(element);
				// close dialog
				dispose();
			}

			// delete table row
		} else if (action.equals("Delete Row")) {
			// get selected row
			int row = dataTable.getSelectedRow();
			if (row >= dataTable.getRowCount() || row < 0)
				return;
			// remove row from table
			element.getListModel().remove(row);
			// force table repaint
			dataTable.revalidate();
			dataTable.repaint();

			// add table row
		} else if (action.equals("Add Row")) {
			// add new row to table
			element.getListModel().addElement(new TitleData());
			// force table repaint
			dataTable.revalidate();
			dataTable.repaint();
		}

		// force repaint of whole window
		element.repaint();
	}
}
