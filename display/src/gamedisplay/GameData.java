package gamedisplay;

import java.awt.image.BufferedImage;
import java.util.ArrayList;

import org.newdawn.slick.Animation;
import org.newdawn.slick.Image;
import org.newdawn.slick.SlickException;

//enum playerstats {HEALTH, DMG, SPEED, AMMO, STRENGTH, NUM_EFFECTS};



public class GameData {
	private int currentScreen;
	private int time;
	private int scoreLimit;
	private ArrayList<Player> players;
	private ArrayList<ActivePowerup> activePowerups;
	private CapturePoint capturepoint;
	
	private static GameData gamedata;
	
	public static GameData getGameData() {
		if( gamedata == null ) gamedata = new GameData();
		return gamedata;
	}
	
	GameData(){
		currentScreen = -1;
		time = -1;
		scoreLimit = -1;
		players = new ArrayList<Player>(); // default 2 players
		players.add(new Player());
		players.add(new Player());
		capturepoint = new CapturePoint();
		activePowerups = new ArrayList<ActivePowerup>();
	}
	GameData(String consoleData){
		updateGameData(consoleData);
	}
	void updateGameData(String consoleData) {
		if(consoleData == null || consoleData.length() == 0) {
			System.out.println("empty data");
			return;
		}
		else {
			System.out.println("updating data");
		}
		int parseIndex = 0; // only up to 8; players will be handled differently
		int playerParseIndex = 0; // only up to 22
		int activesParseIndex = 0; 
		int playerIndex = 0;
		int activesIndex = 0;
		int numberPlayers = 0;
		int numberActive = 0;
		String data = "";
		ArrayList<Integer> effects = new ArrayList<Integer>();
		ArrayList<Integer> timers = new ArrayList<Integer>();
		ArrayList<Integer> weapons = new ArrayList<Integer>();
		boolean playerParse = false;
		boolean activesParse = false;
		for(int index = 0; index < consoleData.length(); index ++) {
			char current = consoleData.charAt(index);
			if(playerParse) {
				if(current == ',') {
					switch(playerParseIndex) {
					case 0:
						players.get(playerIndex).setPlayerid(Integer.parseInt(data));
						break;
					case 1:
						players.get(playerIndex).setLives(Integer.parseInt(data));
						break;
					case 2:
						players.get(playerIndex).setScore(Integer.parseInt(data));
						break;
					case 3:
						players.get(playerIndex).setDirection(Integer.parseInt(data));
						break;
					case 4:
						players.get(playerIndex).setCurrentWeaponM(Integer.parseInt(data));
						break;
					case 5:
						players.get(playerIndex).setCurrentWeaponR(Integer.parseInt(data));
						break;
					case 6:
						//System.out.println("DEBUG: x pos uart str: " + data);
						players.get(playerIndex).setX_pos(Integer.parseInt(data));
						break;
					case 7:
						//System.out.println("DEBUG: y pos uart str: " + data);
						players.get(playerIndex).setY_pos(Integer.parseInt(data));
						break;
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
						effects.add(Integer.parseInt(data));
						break;
					case 13:
						players.get(playerIndex).setEffects(effects);
					case 14:
					case 15:
					case 16:
					case 17:
						timers.add(Integer.parseInt(data));
						break;
					case 18:
						players.get(playerIndex).setTimers(timers);
					case 19:
					case 20:
					case 21:
					case 22:
					case 23:
						weapons.add(Integer.parseInt(data));
						break;
					case 24:
						players.get(playerIndex).setWeapons(weapons);
						players.get(playerIndex).setAttacking(Integer.parseInt(data));
						break;
					case 25:
						players.get(playerIndex).setMoving(Integer.parseInt(data));
					}
					
					data = "";
					if(playerParseIndex >= 25) {
						
						if(playerIndex < players.size() - 1) {
							playerIndex++;
						}
						else {
							playerParse = false;
						}
						effects.clear();
						timers.clear();
						weapons.clear();
						playerParseIndex = 0;
					}
					else {
						playerParseIndex++;
					}
				}
				else if(current != '{' && current != '}' && current != '[' && current != ']'){
					data += current + "";
				}
				
			}
			else if (activesParse) {
				if(current == ',') {
					switch(activesParseIndex) {
					case 0:
						activePowerups.get(activesIndex).setWeaponType(Integer.parseInt(data));
						break;
					case 1:
						activePowerups.get(activesIndex).setHitbox_x(Integer.parseInt(data));
						break;
					case 2:
						activePowerups.get(activesIndex).setHitbox_y(Integer.parseInt(data));
						break;
					case 3:
						activePowerups.get(activesIndex).setX_pos(Integer.parseInt(data));
						break;
					case 4:
						activePowerups.get(activesIndex).setY_pos(Integer.parseInt(data));
						break;
					case 5:
						activePowerups.get(activesIndex).setDir(Integer.parseInt(data));
						break;
					case 6:
						activePowerups.get(activesIndex).setType(Integer.parseInt(data));
						break;
					}
					data = "";
					if(activesParseIndex >= 6) {					
						if(activesIndex < activePowerups.size() - 1) {
							activesIndex++;
						}
						else {
							activesParse = false;
						}
						activesParseIndex = 0;
					}
					else {
						activesParseIndex++;
					}
				}
				else if(current != '{' && current != '}' && current != '[' && current != ']'){
					data += current + "";
				}
			}
			else if(current == ',') {
				switch(parseIndex) {
				case 0:
					currentScreen = Integer.parseInt(data);
					break;
				case 1:
					time = Integer.parseInt(data);
					break;
				case 2:
					scoreLimit = Integer.parseInt(data);
					break;
				case 3:
					numberPlayers = Integer.parseInt(data);
					//players.clear();
					//for(int pindex= 0; pindex < numberPlayers; pindex++) {
					//	players.add(new Player());
					//}
					playerParse = true;
					break;
				case 4:
					capturepoint.setPointid(Integer.parseInt(data));
					break;
				case 5:
					capturepoint.setCurrentPlayerID(Integer.parseInt(data));
					break;
				case 6:
					capturepoint.setPrecentCaptured(Integer.parseInt(data));
					break;
				case 7:
					capturepoint.setX_block(Integer.parseInt(data));
					break;
				case 8:
					capturepoint.setY_block(Integer.parseInt(data));
					break;
				case 9:
					numberActive = Integer.parseInt(data);
					activePowerups.clear();
					for(int aindex = 0; aindex < numberActive; aindex++) {
						activePowerups.add(new ActivePowerup());
					}
					activesParse = true;
					break;
					
				}
				parseIndex++;
				data = "";
			}
			else if(current != '{' && current != '}' && current != '[' && current != ']'){
				data += current + "";
			}
		}
		
	}
	
	public int getCurrentScreen() {
		return currentScreen;
	}


	public void setCurrentScreen(int currentScreen) {
		this.currentScreen = currentScreen;
	}


	public int getTime() {
		return time;
	}


	public void setTime(int time) {
		this.time = time;
	}


	public int getScoreLimit() {
		return scoreLimit;
	}


	public void setScoreLimit(int scoreLimit) {
		this.scoreLimit = scoreLimit;
	}


	public int getNumberPlayers() {
		return players.size();
	}


	public Player getPlayer(int index) {
		return players.get(index);
	}
	public void setPlayers(ArrayList<Player> players) {
		this.players = players;
	}
	public CapturePoint getCapturepoint() {
		return capturepoint;
	}


	public void setCapturepoint(CapturePoint capturepoint) {
		this.capturepoint = capturepoint;
	}
	
	
	public ArrayList<ActivePowerup> getActivePowerups() {
		return activePowerups;
	}


	class CapturePoint{
		private int pointid;
		private int currentPlayerID;
		private int precentCaptured;
		private int x_block;
		private int y_block;
		
		CapturePoint(){
			pointid = -1;
			currentPlayerID = -1;
			precentCaptured = 0;
			x_block = 0;
			y_block = 0;
		}
		
		CapturePoint(int pointid, int currentPlayerID, int precentCaptured,
					 int x_block, int y_block){
			this.pointid = pointid;
			this.currentPlayerID = currentPlayerID;
			this.precentCaptured = precentCaptured;
			this.x_block = x_block;
			this.y_block = y_block;
		}
		public int getPointid() {
			return pointid;
		}
		public void setPointid(int pointid) {
			this.pointid = pointid;
		}
		public int getCurrentPlayerID() {
			return currentPlayerID;
		}
		public void setCurrentPlayerID(int currentPlayerID) {
			this.currentPlayerID = currentPlayerID;
		}
		public int getPrecentCaptured() {
			return precentCaptured;
		}
		public void setPrecentCaptured(int precentCaptured) {
			this.precentCaptured = precentCaptured;
		}
		public int getX_block() {
			return x_block;
		}
		public void setX_block(int x_block) {
			this.x_block = x_block;
		}
		public int getY_block() {
			return y_block;
		}
		public void setY_block(int y_block) {
			this.y_block = y_block;
		}
	}
		
	class ActivePowerup{
		private int type;
		private int weaponType;		// weapon types converted to string
		private int hitbox_x;	// hitbox sizes
		private int hitbox_y;
		private int x;
		private int y;
		private int dir;
		
		ActivePowerup(){
			type = 1;
			weaponType = -1;
			hitbox_x = 0;
			hitbox_y = 0;
			x = 0;
			y = 0;
		}
		ActivePowerup(int type, int weaponType, int hitbox_x, int hitbox_y, int x, int y){
			this.type = type;
			this.weaponType = weaponType;
			this.hitbox_x = hitbox_x;
			this.hitbox_y = hitbox_y;
			this.x = x;
			this.y = y;
		}
		public int getWeaponType() {
			return weaponType;
		}
		public void setWeaponType(int weaponType) {
			this.weaponType = weaponType;
		}
		public int getHitbox_x() {
			return hitbox_x;
		}
		public void setHitbox_x(int hitbox_x) {
			this.hitbox_x = hitbox_x;
		}
		public int getHitbox_y() {
			return hitbox_y;
		}
		public void setHitbox_y(int hitbox_y) {
			this.hitbox_y = hitbox_y;
		}
		public void setX_pos(int x) {
			this.x = x;
		}
		public void setY_pos(int y) {
			this.y = y;
		}
		public int getX_pos() {
			return this.x;
		}
		public int getY_pos() {
			return this.y;
		}
		public int getDir() {
			return this.dir;
		}
		public void setDir(int dir) {
			this.dir = dir;
		}
		public int getType() {
			return type;
		}
		public void setType(int type) {
			this.type = type;
		}
		
	}
	
	class Player{
		private int playerid;
		private int lives;
		private int score;
		private int direction;
		private int currentWeaponM;
		private int currentWeaponR;
		private int x_pos;
		private int y_pos;
		private boolean attacking;
		private ArrayList<Integer> effects;
		private ArrayList<Integer> timers;
		private ArrayList<Integer> weapons;
		private boolean isMoving;
		
		private Animation current, knife, handgun, rifle, shotgun;
		
		Player(){
			playerid = -1;
			lives = -1;
			score = -1;
			direction = 0;
			currentWeaponM = 0;
			currentWeaponR = 0;
			x_pos = 0;
			y_pos = 0;
			effects = new ArrayList<>();
			effects.add(0);
			effects.add(0);
			effects.add(0);
			effects.add(0);
			effects.add(0);
			timers = new ArrayList<>();
			timers.add(0);
			timers.add(0);
			timers.add(0);
			timers.add(0);
			timers.add(0);
			weapons = new ArrayList<>();
			weapons.add(0);
			weapons.add(0);
			weapons.add(0);
			weapons.add(0);
			weapons.add(0);
			weapons.add(0);
			attacking = false;
			isMoving = false;
			initSpriteImages();
		}
		
		//maybe add sprite images to this constructor for different heroes to have images
		Player (int playerid, int lives, int score, int direction, int currentWeaponM, int currentWeaponR,
			int x_pos, int y_pos, ArrayList<Integer> effects, ArrayList<Integer> timers, ArrayList<Integer> weapons, boolean attacking, boolean isMoving){
			this.playerid = playerid;
			this.lives = lives;
			this.score = score;
			this.direction = direction;
			this.currentWeaponM = currentWeaponM;
			this.currentWeaponR = currentWeaponR;
			this.x_pos = x_pos;
			this.y_pos = y_pos;
			setEffects(effects);
			setTimers(timers);
			setWeapons(weapons);
			this.attacking = attacking;
			initSpriteImages();
		}
		private void initSpriteImages() {
			try {
				String path = "maps/sprites/survivor/";
				//Image [] movementImg = {new Image(path + "movement0.png"), new Image(path + "movement1.png"), new Image(path + "movement2.png")};
		        Image [] knifeImg = {new Image(path + "knife0.png"), new Image(path + "knife1.png"), new Image(path + "knife2.png")};
		        Image [] handgunImg = {new Image(path + "handgun0.png"), new Image(path + "handgun1.png"), new Image(path + "handgun2.png")};
		        Image [] rifleImg = {new Image(path + "rifle0.png"), new Image(path + "rifle1.png"), new Image(path + "rifle2.png")};
		        Image [] shotgunImg = {new Image(path + "shotgun0.png"), new Image(path + "shotgun1.png"), new Image(path + "shotgun2.png")};
		        
		        int [] duration = {300, 300, 300};         
		        /*
		         * false variable means do not auto update the animation.
		         * By setting it to false animation will update only when
		         * the user presses a key.
		         */
		        //movement = new Animation(movementImg, duration, false);
		        knife = new Animation(knifeImg, duration, false);
		        handgun = new Animation(handgunImg, duration, false);
		        rifle = new Animation(rifleImg, duration, false);
		        shotgun = new Animation(shotgunImg, duration, false);
	
		        // Original orientation of the sprite. It will look right.
		        current = knife;
			}
			catch (SlickException e) {
	            e.printStackTrace();
	        }
		}
		public void setCurrentSprite() {
			if (attacking) {
			switch (currentWeaponR) {
				case 0:
					current = knife;
					break;
				case 1:
					current = handgun;
					break;
				case 2:
					current = rifle;
					break;
				case 3:
					current = shotgun;
					break;
				}
			}
			else {
				current = knife;
			}
			
		}
		
		public int getPlayerid() {
			return playerid;
		}
		public void setPlayerid(int playerid) {
			this.playerid = playerid;
		}
		public int getLives() {
			return lives;
		}
		public void setLives(int lives) {
			this.lives = lives;
		}
		public int getScore() {
			return score;
		}
		public void setScore(int score) {
			this.score = score;
		}
		public int getDirection() {
			return direction;
		}
		public void setDirection(int direction) {
			this.direction = direction;
		}
		public int getCurrentWeaponM() {
			return currentWeaponM;
		}
		public void setCurrentWeaponM(int currentWeaponM) {
			this.currentWeaponM = currentWeaponM;
		}
		public int getCurrentWeaponR() {
			return currentWeaponR;
		}
		public void setCurrentWeaponR(int currentWeaponR) {
			this.currentWeaponR = currentWeaponR;
		}
		public int getX_pos() {
			return x_pos;
		}
		public void setX_pos(int x_pos) {
			this.x_pos = x_pos;
		}
		public int getY_pos() {
			return y_pos;
		}
		public void setY_pos(int y_pos) {
			this.y_pos = y_pos;
		
		}
		public ArrayList<Integer> getEffects() {
			return effects;
		}

		public void setEffects(ArrayList<Integer> effects) {
			this.effects.clear();
			for(Integer effect: effects) {
				this.effects.add(effect);
			}
		}

		public ArrayList<Integer> getTimers() {
			return timers;
		}

		public void setTimers(ArrayList<Integer> timers) {
			this.timers.clear();
			for(Integer timer: timers) {
				this.timers.add(timer);
			}
		}

		public ArrayList<Integer> getWeapons() {
			return weapons;
		}

		public void setWeapons(ArrayList<Integer> weapons) {
			this.weapons.clear();
			for(Integer weapon: weapons) {
				this.weapons.add(weapon);
			}
		}
		public boolean isAttacking() {
			return attacking;
		}

		public void setAttacking(int attacking) {
			if(attacking == 1) {
				this.attacking = true;
			}
			else {
				this.attacking = false;
			}
		}
		public Animation getCurrentSprite() {
			return current;
		}
		public void setCurrentSpriteDir(int dir) {	
			current.getCurrentFrame().setRotation(dir);
		}
		public boolean isMoving() {
			return isMoving;
		}

		public void setMoving(int isMoving) {
			if(isMoving == 1) {
				this.isMoving = true;
			}
			else {
				this.isMoving = false;
			}
		}
	}
}
