package gui.control;

import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

/**
 * Table model for data values
 * 
 * @author julus
 * 
 */
public class TitleElementTableModel implements TableModel {
	private static final String COLUMN_NAMES[] = { "Name", "Value" };

	// Element associated with
	private TitleElement element;

	/**
	 * Create the table model
	 * 
	 * @param element
	 *            Element parent
	 */
	public TitleElementTableModel(TitleElement element) {
		this.element = element;
	}

	@Override
	public int getRowCount() {
		// number of rows
		return element.getListModel().size();
	}

	@Override
	public int getColumnCount() {
		// number of columns
		return COLUMN_NAMES.length;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		try {
			// get the row data
			TitleData data = element.getListModel().get(rowIndex);
			switch (columnIndex) {
			case 0:
				// get the name
				return data.getName();
			case 1:
				// get the value
				return data.getValue();
			default:
				return null;
			}
		} catch (Exception e) {
			// catch error
			return null;
		}
	}

	@Override
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public Class getColumnClass(int c) {
		// specify all as strings
		return String.class;
	}

	@Override
	public String getColumnName(int columnIndex) {
		try {
			// get the column name
			return COLUMN_NAMES[columnIndex];
		} catch (ArrayIndexOutOfBoundsException aoe) {
			// catch error
			return null;
		}
	}

	@Override
	public boolean isCellEditable(int rowIndex, int columnIndex) {
		// set them all as editable
		return true;
	}

	@Override
	public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
		try {
			// get the row data
			TitleData data = element.getListModel().get(rowIndex);
			switch (columnIndex) {
			case 0:
				// update the name
				data.setName((String) aValue);
				break;
			case 1:
				// update the value
				data.setValue((String) aValue);
				break;
			}
			// force repaint
			element.repaint();
		} catch (Exception e) {
			// catch error
		}
	}

	@Override
	public void addTableModelListener(TableModelListener l) {
	}

	@Override
	public void removeTableModelListener(TableModelListener l) {
	}
}
