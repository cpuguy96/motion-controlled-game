package gamedisplay;


import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.Image;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.geom.Ellipse;
import org.newdawn.slick.geom.Shape;
import org.newdawn.slick.geom.Transform;
import org.newdawn.slick.tiled.TiledMap;


public class PlayerViewScene extends Scene {
	private TiledMap tileMap;
  
    private static int windowWidth = 960, windowHeight = 1080;
    private int mapRenderX = 0, mapRenderY = 0, mapRenderXOff = 0;		//pixel location to start rendering map (for moving map)
    //private int mapSizeX = 0, mapSizeY = 0;
    
    private int playerid;

    private static final int TEXTURE_SIZE = 32;
    private Image health;
    
	

    public PlayerViewScene(int pid) {
    	super();
    	playerid = pid;
    	mapRenderXOff = windowWidth * playerid;
    }
    
	public void init(GameContainer gc) throws SlickException  {
        tileMap = new TiledMap("maps/map_and_tiles/forest_map.tmx");
        System.out.println("map block w:" + tileMap.getWidth() + " h:" + tileMap.getHeight());
        //System.out.println(mapToTM4C()); while(true) {}		//COMMENT THIS
	}
	
    String mapToTM4C() {
  	   String s = "const char MAP_LAYOUT [MAP_Y_SIZE][MAP_X_SIZE] = {\n";
  	   // build a collision map based on tile properties in the TileD map

  	   for (int yAxis=0; yAxis < tileMap.getHeight(); yAxis++) {
  			s+= "\"";
  			for (int xAxis=0; xAxis < tileMap.getWidth(); xAxis++) {
  				boolean b = false;
  				boolean slow = false;
  				for (int i = 0; i < tileMap.getLayerCount(); i++) {		//we have a buggy layer.
  					
	  				Integer id = tileMap.getTileId(xAxis, yAxis, i);
	  				if (id != null) {
	  					String bStr = "", slowStr = "";
	  					try {
	  						bStr = tileMap.getTileProperty(id, "blocked", "false");
	  						slowStr = tileMap.getTileProperty(id, "slow", "false");
	  					}
	  					catch(Exception e) {
	  						e.printStackTrace();
	  						System.out.println("xAxis=" + xAxis + ", yAxis=" + yAxis + ", layer=" + i);
	  					}
	  					if (bStr != null && bStr.equals("true")) {
	  						b = true;
	  					}
	  					if (slowStr != null && slowStr.equals("true")) {
	  						slow = true;
	  					}
	  				}
  				}
  				if (b) {
  					s += "X";
  				}
  				else if (slow) {
  					s += "S";
  				}
  				else {
  					s += "O";
  				}
  			}
  			s+= "\",\n";
  		}
  		s+= "};";
  		
  		return s;
     }
    
	protected void CustomRender(GameContainer gc, Graphics g) throws SlickException  {
	 	//moving the widow 
		//System.out.println("map x=" + mapRenderX + " y=" + mapRenderY);
		//System.out.println("window w=" + windowWidth + " h=" + windowHeight);
		
		int x_temp = 0;
		if (mapRenderX < 0) {
			x_temp = (-1 * mapRenderX) / 32 + 1;
		}
		int y_temp = 0;
		if (mapRenderY < 0) {
			y_temp = (-1 * mapRenderY) / 32 + 1;
		}
		
		int mapZeroX = mapRenderX + mapRenderXOff ;
		int mapZeroY = mapRenderY;
		
    	tileMap.render(mapZeroX + x_temp * 32, mapZeroY + y_temp * 32, x_temp, y_temp, (windowWidth  - x_temp * 32 - mapRenderX) / 32, (windowHeight - y_temp * 32 - mapRenderY) / 32);
    	GameData.getGameData().getPlayer(playerid).getCurrentSprite().draw(mapRenderXOff + windowWidth / 2,  windowHeight / 2);
    	
    	//rendering other players, can use this logic for powerups and capture points too
    	for (int i = 0; i < GameData.getGameData().getNumberPlayers(); i++) {
    		if (i != playerid) {
    			GameData.Player otherP = GameData.getGameData().getPlayer(i);
    			//get sprite from player object
    			if (checkRenderBounds(mapZeroX + otherP.getX_pos(), 32)) {
    				otherP.getCurrentSprite().draw(mapZeroX + otherP.getX_pos(), mapZeroY + otherP.getY_pos());
    			}
    		}
    	}
    	
    	//render powerups
    	if (GameData.getGameData().getActivePowerups() != null) {
	    	for (GameData.ActivePowerup powerup : GameData.getGameData().getActivePowerups()) {
	    		if (checkRenderBounds(powerup.getX_pos() + mapZeroX, powerup.getHitbox_x())) {
		    		String path = "maps/imgs/";
		    		switch(powerup.getType()) {
		    		case 0:	// health=
						Image healthImg = new Image(path + "health.png");
						healthImg.draw(powerup.getX_pos() + mapZeroX, powerup.getY_pos() + mapZeroY);
		    			break;
		    		case 1: // damage
		    			g.setColor(Color.black);
		    			g.fillOval(powerup.getX_pos() + mapZeroX, powerup.getY_pos() + mapZeroY, powerup.getHitbox_x(), powerup.getHitbox_y());
		    			break;
		    		case 2: // speed
		    			Image speedImg = new Image(path + "speed.png");
		    			speedImg.draw(powerup.getX_pos() + mapZeroX, powerup.getY_pos() + mapZeroY);
		    			break;
		    		case 3: // ammo
		    			Image ammoImg = new Image(path + "ammo.png");
		    			ammoImg.draw(powerup.getX_pos() + mapZeroX, powerup.getY_pos() + mapZeroY);
		    			break;
		    		case 4: // strength
		    			Image strImg = new Image(path + "strength.png");
		    			strImg.draw(powerup.getX_pos() + mapZeroX, powerup.getY_pos() + mapZeroY);
		    			break;
		    		}
	    		}
    			//System.out.println("powerup at x:" + powerup.getX_pos() + " y:" + powerup.getY_pos() + " dir:" + powerup.getDir());
    			
    			/*//not doing this for now because collision on model side becomes harder. however this idea works for rotating if the hitboxes match
				int powerupx = powerup.getX_pos() + mapZeroX + powerup.getHitbox_x() / 2;
				int powerupy = powerup.getY_pos() + mapZeroY + powerup.getHitbox_y() / 2;
    			Shape rangeShape = new Ellipse(powerupx, powerupy, powerup.getHitbox_x(), powerup.getHitbox_y());
    			rangeShape = rangeShape.transform(Transform.createRotateTransform((float) Math.toRadians(powerup.getDir()), powerupx - powerup.getHitbox_x() / 2, powerupy));
    			g.fill(rangeShape);*/
    			
    			//	
    			
	    		
	    		
	    	}
			g.setColor(Color.white);
    	}
    	
    	
    	//render active dmg array
    	/* for (int i = 0; i < GameData.getGameData().getNumberPlayers(); i++) {
    		
    		if (GameData.getGameData().getPlayer(i).isAttacking()) {
    			System.out.println("Player " + i + " is attacking");
    			int dir = GameData.getGameData().getPlayer(i).getDirection();
    			
    			int attackX = GameData.getGameData().getPlayer(i).getX_pos() + mapRenderX + mapRenderXOff; 
    			int attackY = GameData.getGameData().getPlayer(i).getY_pos() + mapRenderY; 
    			if (dir >= 45 && dir < 135) {
    				attackY -= TEXTURE_SIZE;
    			}
    			else if (dir >= 135 && dir < 225) {
    				attackX += TEXTURE_SIZE;
    			}
    			else if (dir >= 225 && dir < 315) {
    				attackY += TEXTURE_SIZE;
    			}
    			else if (dir >= 315 || dir < 45) {
    				attackX -= TEXTURE_SIZE;
    			}
    			g.setColor(Color.red);
    			g.fillOval(attackX + TEXTURE_SIZE / 4, attackY + TEXTURE_SIZE / 4, TEXTURE_SIZE / 2, TEXTURE_SIZE / 2);
    			g.setColor(Color.white);
    		}
    	}*/
	}
	
	private boolean checkRenderBounds(int x, int w) {
		return x >= mapRenderXOff && (x + w) < (mapRenderXOff + windowWidth);
	}
	
	protected void CustomUpdate(GameContainer container, int delta) throws SlickException  {	
		updateMapRenderLoc();
		updateAnimation(delta);
	}
	
	public String toString() {
		return playerid + "gameview";
	}
	
	private void updateAnimation(int delta) {
		//update visibility of stuff or anything that is too much to happen in render, however most update stuff is already handled by tm4c
		//most information is handled by the pointer to the player object (called player)
		
		GameData.getGameData().getPlayer(playerid).setCurrentSpriteDir(GameData.getGameData().getPlayer(playerid).getDirection() + 180);		//+180 due to the images
		GameData.getGameData().getPlayer(playerid).setCurrentSprite();
		if (GameData.getGameData().getPlayer(playerid).isMoving()) {
			GameData.getGameData().getPlayer(playerid).getCurrentSprite().update(delta);
		}
		else {
			GameData.getGameData().getPlayer(playerid).getCurrentSprite().setCurrentFrame(0);
		}
		
	}
	

	private void updateMapRenderLoc() {		
		mapRenderX = -1 * GameData.getGameData().getPlayer(playerid).getX_pos() + windowWidth / 2;
		mapRenderY = -1 * GameData.getGameData().getPlayer(playerid).getY_pos() + windowHeight / 2;
	}
	    
	
	//if we can resize the widow we need to know the size of this view 
    private void updateWindowSize(int width, int height) {
    	windowWidth = width;
    	windowHeight = height;
    }
    

    
}
