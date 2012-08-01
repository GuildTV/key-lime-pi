package gui.control;

import gui.MainFrame;

import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;

public class TitlePanel extends JPanel {
	private static final long serialVersionUID = 8021157860217699741L;

	private static final int ARRAY_INCREMENT = 10;

	private MainFrame owner;

	private Object arrayLock = new Object();

	private JScrollPane pane;
	private JPanel panel;

	private TitleElement elements[];
	private int elmCount = 0;

	public TitlePanel(MainFrame owner) {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Title Control"));

		this.owner = owner;

		panel = new JPanel();
		//panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		pane = new JScrollPane(panel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		add(pane, BorderLayout.CENTER);

		elements = new TitleElement[ARRAY_INCREMENT];

	}

	public void create() {
		TitleElement elm = new TitleElement(this, owner);
		addElm(elm);
	}

	private void addElm(TitleElement elm) {
		synchronized (arrayLock) {
			try {
				elements[elmCount] = elm;
			} catch (ArrayIndexOutOfBoundsException aoe) {
				TitleElement elms[] = new TitleElement[elements.length + ARRAY_INCREMENT];
				System.arraycopy(elements, 0, elms, 0, elmCount);
				elms[elmCount] = elm;
				elements = elms;
			}
		}
		panel.add(elm);

	}

	public void removeElm(TitleElement elm) {
		synchronized (arrayLock) {
			int i;
			for (i = 0; i < elements.length; i++) {
				if (elements[i] == elm)
					break;
			}

			if (i >= elements.length)
				return;

			removeElm(i);
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
		}
	}

	public void reset() {
		elements = new TitleElement[ARRAY_INCREMENT];
		panel.removeAll();

		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				create();
			}
		});
		// create();
		// create();
	}
}
