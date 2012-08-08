package gui.control;

import gui.MainFrame;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class ConnectionPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = -1682827000691560614L;

	private MainFrame owner;
	
	private JTextField masterStatus; 
	private JTextField slaveStatus;
	
	private JButton masterButton; 
	private JButton slaveButton;
	
	private JTextField currentAction;
	
	private JButton liveButton;
	
	public ConnectionPanel(MainFrame owner) {
		super(new GridBagLayout());
		setBorder(BorderFactory.createTitledBorder("Pi/Connection Status"));
		
		this.owner = owner;
		
		JLabel masterLabel = new JLabel("master: ");
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 0;
		c.insets = new Insets(0,0,2,0);
		add(masterLabel, c);
		
		masterStatus = new JTextField(10);
		masterStatus.setText("disconnected");
		masterStatus.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 0;
		c.insets = new Insets(0,0,2,0);
		add(masterStatus, c);
		
		masterButton = new JButton("Connect");
		masterButton.setActionCommand("master-button");
		masterButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 2;
		c.gridy = 0;
		c.insets = new Insets(0,4,2,0);
		add(masterButton, c);
		
		JLabel slaveLabel = new JLabel("slave: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(2,0,0,0);
		add(slaveLabel, c);
		
		slaveStatus = new JTextField(10);
		slaveStatus.setText("disconnected");
		slaveStatus.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 1;
		c.insets = new Insets(2,0,0,0);
		add(slaveStatus, c);
		
		slaveButton = new JButton("Connect");
		slaveButton.setActionCommand("slave-button");
		slaveButton.addActionListener(this);
		slaveButton.setEnabled(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 2;
		c.gridy = 1;
		c.insets = new Insets(2,4,0,0);
		add(slaveButton, c);
		
		currentAction = new JTextField(20);
		currentAction.setText("disconnected");
		currentAction.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 3;
		c.gridy = 0;
		c.gridheight = 2;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.insets = new Insets(1,10,1,10);
		add(currentAction, c);
		
		liveButton = new JButton("Go Live");
		liveButton.setEnabled(false);
		liveButton.setActionCommand("live-button");
		liveButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 4;
		c.gridy = 0;
		c.gridheight = 2;
		c.insets = new Insets(1,1,1,1);
		add(liveButton, c);
	}
	
	public void reset(){
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("master-button")){
			if(owner.getControl().isMasterConnected()){
				
			} else 
				owner.getControl().showConnect();
		} else if(e.getActionCommand().equals("slave-button")){
			
		} else if(e.getActionCommand().equals("live-button")){
			
		}
	}
	
	public void setMasterConnected(boolean state) {
		if(state){
			masterStatus.setText("connected");
			masterButton.setText("Disconnect");
			currentAction.setText("waiting");
		} else {
			masterStatus.setText("disconnected");
			masterButton.setText("Connect");
			currentAction.setText("disconnected");
			setSlaveConnected(false);
		}
	}
	
	public void setSlaveConnected(boolean state) {
		if(state){
			slaveStatus.setText("connected");
			slaveButton.setText("Disconnect");
		} else {
			slaveStatus.setText("disconnected");
			slaveButton.setText("Connect");
		}
	}
	
	public void setAction(String text){
		currentAction.setText(text);
	}
}