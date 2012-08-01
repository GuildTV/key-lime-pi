package gui.control;

import gui.BottomBorder;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class TitleButtonPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = -2204407876186244418L;

	private TitlePanel owner;

	private JButton addButton;
	private JButton deleteButton;
	private JButton cloneButton;
	private JButton editButton;
	private JButton moveUpButton;
	private JButton moveDownButton;

	public TitleButtonPanel(TitlePanel owner) {
		this.owner = owner;

		setBorder(new BottomBorder(Color.GRAY));

		addButton = new JButton("Add");
		addButton.addActionListener(this);
		addButton.setActionCommand("Add");
		add(addButton);

		editButton = new JButton("Edit");
		editButton.addActionListener(this);
		editButton.setActionCommand("Edit");
		add(editButton);

		cloneButton = new JButton("Clone");
		cloneButton.addActionListener(this);
		cloneButton.setActionCommand("Clone");
		add(cloneButton);

		deleteButton = new JButton("Delete");
		deleteButton.addActionListener(this);
		deleteButton.setActionCommand("Delete");
		add(deleteButton);

		moveUpButton = new JButton("Move Up");
		moveUpButton.addActionListener(this);
		moveUpButton.setActionCommand("Move Up");
		add(moveUpButton);

		moveDownButton = new JButton("Move Down");
		moveDownButton.addActionListener(this);
		moveDownButton.setActionCommand("Move Down");
		add(moveDownButton);

		setNull();
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("Add")) {
			TitleElement elm = owner.create();
			owner.revalidate();
			owner.repaint();
			TitleElementEditDialog d = new TitleElementEditDialog(owner.getOwner(), elm);
			d.setVisible(true);

		} else if (e.getActionCommand().equals("Delete")) {
			if (owner.getSelectedElement() == null)
				return;
			Object[] options = { "Yes", "No" };
			int n = JOptionPane.showOptionDialog(owner, "Are you sure you want to delete this?", "Confirm Deletion",
					JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[1]);
			if (n == JOptionPane.YES_OPTION) {
				owner.removeElm(owner.getSelectedElement());
			}

		} else if (e.getActionCommand().equals("Clone")) {
			if (owner.getSelectedElement() == null)
				return;
			TitleElement elm = new TitleElement(owner, owner.getSelectedElement());
			owner.addElm(elm);

		} else if (e.getActionCommand().equals("Edit")) {
			if (owner.getSelectedElement() == null)
				return;
			TitleElementEditDialog d = new TitleElementEditDialog(owner.getOwner(), owner.getSelectedElement());
			d.setVisible(true);

		} else if (e.getActionCommand().equals("Move Up")) {
			if (owner.getSelectedElement() == null)
				return;

			owner.moveUpElm(owner.getSelectedElement());
			owner.redrawElms();

		} else if (e.getActionCommand().equals("Move Down")) {
			if (owner.getSelectedElement() == null)
				return;

			owner.moveDownElm(owner.getSelectedElement());
			owner.redrawElms();
		}

		owner.revalidate();
		owner.repaint();
	}

	public void setNull() {
		deleteButton.setEnabled(false);
		cloneButton.setEnabled(false);
		editButton.setEnabled(false);
		moveUpButton.setEnabled(false);
		moveDownButton.setEnabled(false);
	}

	public void setElm() {
		deleteButton.setEnabled(true);
		cloneButton.setEnabled(true);
		editButton.setEnabled(true);
		moveUpButton.setEnabled(true);
		moveDownButton.setEnabled(true);
	}
}
