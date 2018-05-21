package gamedisplay;


import java.awt.Font;
import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.TrueTypeFont;


public class GameStatusScene extends Scene {
	
    private static int windowWidth = 1920, windowHeight = 1080;
	private static int wordHeight = 20;
	private int wordHeightBig = 60;

	private int lastWinner;
	
	TrueTypeFont  font;
	
	protected void CustomRender(GameContainer gc, Graphics g) throws SlickException  {
		for (int i = 0; i < GameData.getGameData().getNumberPlayers(); i++) {
			renderPlayerStats(g, i);
		}
		renderGameStats(g);
	}
	
	
	protected void CustomUpdate(GameContainer gc, int t) throws SlickException  {}
	
	public void init(GameContainer gc) throws SlickException  {
		font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeight), false);
	}
	
	
	public String toString() {
		return "GameStatusScene";
	}
	
	
	//stuff in top middle
	private void renderGameStats(Graphics g) {
		if (GameData.getGameData().getCurrentScreen() == 1) {
			//pause
			g.setColor(Color.black);
			g.fillRect(0, 0, windowWidth, windowHeight);
			g.setColor(Color.white);
			font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeightBig), false);
			font.drawString(windowWidth / 2 - 50, windowHeight / 2, "GAME PAUSED", Color.white);
			font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeight), false);
		}
		else if (gameOver()) {
			g.setColor(Color.black);
			g.fillRect(0, 0, windowWidth, windowHeight);
			g.setColor(Color.white);
			
			font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeightBig), false);
			font.drawString(windowWidth / 4, windowHeight / 2 - wordHeightBig, "GAME OVER, PLAYER " + (lastWinner + 1) + " WINS!", Color.white);
			font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeightBig), false);
			font.drawString(windowWidth / 4, windowHeight / 2 + wordHeightBig, "THANKS FOR PLAYING!", Color.white);
			font = new TrueTypeFont (new Font("SansSerif", Font.PLAIN, wordHeight), false);
		}
		else {
			int lvl = 1;
			int boxWidth = 400;
			int xPos = windowWidth / 2 - boxWidth / 2;
			
			// add this for a new line 
			//font.drawString("", xPos, wordHeight * lvl++);
			
			g.setColor(Color.black);
			g.fillRect(0, 0, windowWidth, 6*wordHeight + 5);
			g.setColor(Color.white);
			
			//GameData.Player p0 = GameData.getGameData().getPlayer(0);
			//GameData.Player p1 = GameData.getGameData().getPlayer(1);
			
			font.drawString((float) xPos, (float) wordHeight * lvl++, "Welcome to the Game!", Color.white);
			font.drawString(xPos, wordHeight * lvl++, "UT EE445L Final Project", Color.white);
			font.drawString(xPos, wordHeight * lvl++, "Cole Thompson & Chimezie Iwuanyanwu", Color.white);
			//font.drawString(xPos, wordHeight * lvl++, "Score | P0:" + p0.getScore() + "/" + GameData.getGameData().getScoreLimit() + " | P1:" + p1.getScore() + "/" + GameData.getGameData().getScoreLimit(), Color.white);
			font.drawString(xPos, wordHeight * lvl++, "Time  | " + GameData.getGameData().getTime(), Color.white);
			font.drawString(xPos, wordHeight * lvl++, "", Color.white);
			
			//border
			g.drawRect(xPos - 10, wordHeight - 10, boxWidth, (lvl - 1)*wordHeight + 15);
			//draw line down middle of screen
		 	g.fillRect(windowWidth / 2 - 2, wordHeight * lvl + 5, 4, windowHeight);
		}
	}
	
	private boolean gameOver() {
		if (GameData.getGameData().getPlayer(0).getEffects().get(0) <= 0) {
			lastWinner = 0;
			return true;
		}
		else if (GameData.getGameData().getPlayer(1).getEffects().get(0) <= 0) {
			lastWinner = 1;
			return true;
		}
		return false;
	}
	
	//stuff near player windows
	private void renderPlayerStats(Graphics g, int playerid) {
		GameData.Player p = GameData.getGameData().getPlayer(playerid);
		
		int boxWidth = 250;
		int numLines = 5;	//edit this if add more lines
		int xPos = 20 + playerid * (windowWidth - boxWidth - 40);
		int lvl = 1;
		
		
		
		// add this for a new line 
		//g.drawString("", xPos, windowHeight - (2 + lvl++)*wordHeight);

		int hp = 0;
		int speed = 0;
		int ammo = 0;
		int strength = 0;
		if (p.getEffects().size() > 0) {
			hp = p.getEffects().get(0);
			speed = p.getEffects().get(2) / 32;
			ammo = p.getEffects().get(3);
			strength = p.getEffects().get(4);
		}
		
		g.setColor(Color.black);
		g.fillRect(xPos - 10, windowHeight - (numLines + 2)*wordHeight - 10, boxWidth, (numLines + 1)*wordHeight + 20);
		g.setColor(Color.white);
		g.drawRect(xPos - 10, windowHeight - (numLines + 2)*wordHeight - 10, boxWidth, (numLines + 1)*wordHeight + 20);

		font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Speed: " + speed + " block(s)/sec", Color.white);
		font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Strength: " + strength, Color.white);
		font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Ammo: " + ammo, Color.white);
		font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Weapon: " + weaponNumToString(p.getCurrentWeaponR()), Color.white);
		//font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Score: " + p.getScore(), Color.white);
		font.drawString(xPos, windowHeight - (2 + lvl++)*wordHeight, "Health: " + hp, Color.white);

	}
	
	private String weaponNumToString(int num) {
		String s = "";
		switch(num) {
		case 0:
			s = "Melee";
			break;
		case 1:
			s = "Handgun";
			break;
		case 2:
			s = "Rifle";
			break;
		case 3:
			s = "Shotgun";
			break;
		default:
			s = "Melee";
			break;
		}
		return s;
	}
}
