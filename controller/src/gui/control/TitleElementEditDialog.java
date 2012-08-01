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

public class TitleElementEditDialog extends JDialog implements ActionListener {
	private static final long serialVersionUID = 8900911924444729087L;

	private MainFrame owner;
	private TitleElement element;
	private JTextField nameField;
	private JComboBox<String> scriptNameField;
	private JTable dataTable;
	private JScrollPane tablePanel;

	private TitleElementTableModel tableModel;

	public TitleElementEditDialog(MainFrame owner, final TitleElement element) {
		super(owner);
		this.element = element;
		this.owner = owner;

		setTitle("Edit Selected Title");
		setModal(true);
		setSize(500, 200);
		setResizable(false);
		JPanel panel = new JPanel(new GridBagLayout());
		add(panel);

		JLabel title = new JLabel("Edit Title Element");
		title.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 4;
		c.insets = new Insets(5, 5, 5, 5);
		panel.add(title, c);

		JLabel label = new JLabel("Name: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(label, c);

		nameField = new JTextField(element.getNameValue(), 20);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1.0;
		c.gridwidth = 2;
		c.gridx = 1;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(nameField, c);

		JButton save = new JButton("Save Name");
		save.addActionListener(this);
		save.setActionCommand("Set Name");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 3;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(save, c);

		JLabel label2 = new JLabel("Script/Video: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 2;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(label2, c);

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

		JButton addRow = new JButton("Add Row");
		addRow.addActionListener(this);
		addRow.setActionCommand("Add Row");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 0;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(addRow, c);

		JButton delRow = new JButton("Delete Row");
		delRow.addActionListener(this);
		delRow.setActionCommand("Delete Row");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 1;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(delRow, c);

		JButton delete = new JButton("Delete");
		delete.addActionListener(this);
		delete.setActionCommand("Delete");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 2;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(delete, c);

		JButton close = new JButton("Close");
		close.addActionListener(this);
		close.setActionCommand("Close");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 3;
		c.gridy = 4;
		c.insets = new Insets(0, 5, 5, 5);
		panel.add(close, c);

		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent event) {
				dispose();
			}
		});
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();
		if (action.equals("Set Name")) {
			element.setNameValue(nameField.getText());
		} else if (action.equals("Set Script")) {
			element.setScriptName((String) scriptNameField.getSelectedItem());
		} else if (action.equals("Close"))
			dispose();
		else if (action.equals("Delete")) {
			Object[] options = {"Yes", "No"};
			int n = JOptionPane.showOptionDialog(owner,
				    "Are you sure you want to delete this?",
				    "Confirm Deletion",
				    JOptionPane.YES_NO_OPTION,
				    JOptionPane.WARNING_MESSAGE,
				    null,
				    options,
				    options[1]);
			if(n == JOptionPane.YES_OPTION) {
				element.getMyParent().removeElm(element);
				dispose();
			}
		} else if (action.equals("Delete Row")) {
			int row = dataTable.getSelectedRow();
			if(row >= dataTable.getRowCount() || row < 0)
				return;
			element.getListModel().remove(row);
			dataTable.revalidate();
			dataTable.repaint();
		} else if (action.equals("Add Row")) {
			element.getListModel().addElement(new TitleData());
			dataTable.revalidate();
			dataTable.repaint();
		}
		element.repaint();
	}
}
