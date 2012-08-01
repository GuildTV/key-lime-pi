package gui.control;

import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

public class TitleElementTableModel implements TableModel {
	private static final String COLUMN_NAMES[] = { "Name", "Value" };

	private TitleElement element;

	public TitleElementTableModel(TitleElement element) {
		this.element = element;
	}

	@Override
	public int getRowCount() {
		return element.getListModel().size();
	}

	@Override
	public int getColumnCount() {
		return COLUMN_NAMES.length;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		try {
			TitleData data = element.getListModel().get(rowIndex);
			switch (columnIndex) {
			case 0:
				return data.getName();
			case 1:
				return data.getValue();
			default:
				return null;
			}
		} catch (Exception e) {
			return null;
		}
	}

	@Override
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public Class getColumnClass(int c) {
		return String.class;
	}

	@Override
	public String getColumnName(int columnIndex) {
		try {
			return COLUMN_NAMES[columnIndex];
		} catch (ArrayIndexOutOfBoundsException aoe) {
			return null;
		}
	}

	@Override
	public boolean isCellEditable(int rowIndex, int columnIndex) {
		return true;
	}

	@Override
	public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
		try {
			TitleData data = element.getListModel().get(rowIndex);
			switch (columnIndex) {
			case 0:
				data.setName((String) aValue);
				break;
			case 1:
				data.setValue((String) aValue);
				break;
			}
			element.repaint();
		} catch (Exception e) {
		}
	}

	@Override
	public void addTableModelListener(TableModelListener l) {
	}

	@Override
	public void removeTableModelListener(TableModelListener l) {
	}
}
