package gui.control;

import gui.MainFrame;

import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

public class TitlePanel extends JPanel {
	private static final long serialVersionUID = 8021157860217699741L;

	private static final int ARRAY_INCREMENT = 10;

	private MainFrame owner;

	private Object arrayLock = new Object();

	private JScrollPane pane;
	private JPanel panel;

	private TitleButtonPanel buttonPanel;

	private TitleElement elements[];
	private int elmCount = 0;

	private TitleElement selected;

	public TitlePanel(MainFrame owner) {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Title Control"));

		this.owner = owner;

		buttonPanel = new TitleButtonPanel(this);
		add(buttonPanel, BorderLayout.NORTH);

		panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		pane = new JScrollPane(panel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		pane.setBorder(BorderFactory.createEmptyBorder());
		add(pane, BorderLayout.CENTER);

		elements = new TitleElement[ARRAY_INCREMENT];

	}

	public TitleElement create() {
		TitleElement elm = new TitleElement(this);
		addElm(elm);
		return elm;
	}

	public void addElm(TitleElement elm) {
		synchronized (arrayLock) {
			try {
				elements[elmCount] = elm;
				elmCount++;
			} catch (ArrayIndexOutOfBoundsException aoe) {
				TitleElement elms[] = new TitleElement[elements.length + ARRAY_INCREMENT];
				System.arraycopy(elements, 0, elms, 0, elmCount);
				elms[elmCount] = elm;
				elmCount++;
				elements = elms;
			}
		}
		panel.add(elm);
	}

	public void removeElm(TitleElement elm) {
		synchronized (arrayLock) {
			removeElm(getPosElm(elm));
		}
	}

	public int getPosElm(TitleElement elm) {
		synchronized (arrayLock) {
			int i;
			for (i = 0; i < elmCount; i++) {
				if (elements[i] == elm)
					break;
			}

			if (i >= elmCount)
				return -1;

			return i;
		}
	}

	public void moveUpElm(TitleElement elm) {
		synchronized (arrayLock) {
			int pos = getPosElm(elm);
			if (pos <= 0)
				return;

			elements[pos] = elements[pos - 1];
			elements[pos - 1] = elm;
		}
	}

	public void moveDownElm(TitleElement elm) {
		synchronized (arrayLock) {
			int pos = getPosElm(elm);
			if (pos < 0 || pos == elmCount - 1)
				return;

			elements[pos] = elements[pos + 1];
			elements[pos + 1] = elm;
		}
	}

	public void removeElm(int i) {
		if (i > elmCount || i < 0)
			return;

		panel.remove(elements[i]);

		synchronized (arrayLock) {
			TitleElement elms[] = new TitleElement[elements.length];
			System.arraycopy(elements, 0, elms, 0, i);
			System.arraycopy(elements, i, elms, i + 1, elements.length - i - 1);
			elements = elms;
			elmCount--;
		}
	}

	public void redrawElms() {
		synchronized (arrayLock) {
			panel.removeAll();
			for (int i = 0; i < elmCount; i++) {
				panel.add(elements[i]);
			}
		}
	}

	public MainFrame getOwner() {
		return owner;
	}

	public void reset() {
		elements = new TitleElement[ARRAY_INCREMENT];
		panel.removeAll();
		selected = null;
	}

	public void setSelectedElement(TitleElement elm) {
		selected = elm;
		if (elm == null)
			buttonPanel.setNull();
		else
			buttonPanel.setElm();
	}

	public TitleElement getSelectedElement() {
		return selected;
	}
}
