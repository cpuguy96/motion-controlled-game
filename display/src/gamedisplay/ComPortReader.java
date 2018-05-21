package gamedisplay;

import com.fazecast.jSerialComm.*;

public class ComPortReader {
	private boolean readingData;
	private String recData;
	private String endMessage;
	private String tempData;
	private char startMessage[];
	private SerialPort comPort;
	
	ComPortReader(){
		recData = "";
		startMessage = new char[5];
		startMessage[0] = '+';
		startMessage[1] = 'G';
		startMessage[2] = 'A';
		startMessage[3] = 'M';
		startMessage[4] = 'E';
		endMessage = "GAME-";
		readingData = false;
		portInit();
	}
	ComPortReader(char startMessage[], String endMessage){
		this.startMessage = startMessage;
		this.endMessage = endMessage;
		portInit();
	}
	
	private void portInit() {
		comPort = SerialPort.getCommPorts()[0];
		comPort.setFlowControl(SerialPort.FLOW_CONTROL_XONXOFF_IN_ENABLED | SerialPort.FLOW_CONTROL_XONXOFF_OUT_ENABLED);
	    comPort.setComPortParameters(115200, 8,
	                                 SerialPort.ONE_STOP_BIT, SerialPort.NO_PARITY);
		comPort.openPort();
		comPort.addDataListener(new SerialPortDataListener() {
		   @Override
		   public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
		   @Override
		   public void serialEvent(SerialPortEvent event)
		   {
		      if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
		         return;
		      byte[] newData = new byte[comPort.bytesAvailable()];
		      comPort.readBytes(newData, newData.length);
		      for(int i = 0; i < newData.length; ++i) {
		    	  if(!readingData) {
		    		  startMessage[0] = startMessage[1];
		    		  startMessage[1] = startMessage[2];
		    		  startMessage[2] = startMessage[3];
		    		  startMessage[3] = startMessage[4];
		    		  startMessage[4] = ((char)newData[i]);
		    		  if(startMessage[0] == '+' && startMessage[1] == 'G' && startMessage[2] == 'A' && startMessage[3] == 'M' && startMessage[4] == 'E') {
		    			  tempData = "";
		    			  readingData = true;
		    		  }
		    	  }
		    	  else {
		    		  tempData += ((char)newData[i]);
		    		  int dataLength = tempData.length();
		    		  if(dataLength >= 5) {
		    			  if(tempData.substring(dataLength - 5, dataLength).equals(endMessage)) {
		    				  readingData = false;
			    			  recData = tempData.substring(0, dataLength - 5);
			    		  }
		    		  }
		    		  
		    	  }
		      }
		   }
		});
	}
	
	public String getRecData() {
		return recData;
	}
	
	public void setStartMessage(char[] startMessage) {
		this.startMessage = startMessage;
	}
	public void setEndMessage(String endMessage) {
		this.endMessage = endMessage;
	}
}
	