package gamedisplay;

public class ParseTester {
	static private GameData gameData;		//most recent game data
	static private ComPortReader UARTReader;
	
	public static void main(String[] args) {
		gameData = new GameData();
//		UARTReader = new ComPortReader();
		String data = "{2,11196,100,2,0,5,0,1,1,1,672,608,100,0,64,100,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,5,0,0,1,1,1088,608,100,0,64,100,1,0,0,0,0,0,1,0,0,0,0,0,0,0,-1,-1,0,0,07,-1,32,32,256,224,0,3,-1,32,32,224,1056,0,4,-1,32,32,896,608,0,0,-1,32,32,928,224,0,3,-1,32,32,928,992,0,3,-1,32,32,1344,224,0,4,-1,32,32,1504,1216,0,2,}";
			if(data.length() != 0) {
				gameData.updateGameData(data);
				System.out.println("Parsed?");
			}
		
	}

}





