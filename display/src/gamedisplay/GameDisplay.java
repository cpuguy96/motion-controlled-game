package gamedisplay;


import org.newdawn.slick.AppGameContainer;
import org.newdawn.slick.BasicGame;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.ScalableGame;


public class GameDisplay extends BasicGame
{

    private static int windowWidth = 1920, windowHeight = 1080;

	static private ComPortReader UARTReader;
    public GameDisplay()
    {
        super("Fighting Game");
    }

    public static void main(String [] arguments) {
        try {
        	ScalableGame game = new ScalableGame(new GameDisplay() , windowWidth, windowHeight , false);
        	AppGameContainer app = new AppGameContainer(game);
            app.setDisplayMode(windowWidth, windowHeight, false);
	        app.setVSync(true);
	        app.setShowFPS(false);
	        UARTReader = new ComPortReader();
            app.start();
        }
        catch (SlickException e)
        {
            e.printStackTrace();
        }
    }

    @Override
    public void init(GameContainer container) throws SlickException {
    	SceneManager.getManager().setGameContainer(container);
		SceneManager.getManager().addScene( new PlayerViewScene(1) );
		SceneManager.getManager().addScene( new PlayerViewScene(0) );
		SceneManager.getManager().addScene( new GameStatusScene() );
		
		//this is how we add more screens, like pause and score and such
    }
		
	@Override
	public void update(GameContainer container, int delta) throws SlickException {
		String data = UARTReader.getRecData();
		try {
		GameData.getGameData().updateGameData(data);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		for(int index = 0; index < 2; index++) {
			System.out.println("Player: " + index + " x: " + GameData.getGameData().getPlayer(index).getX_pos() + " y: " + GameData.getGameData().getPlayer(index).getY_pos() + " dir:" + GameData.getGameData().getPlayer(index).getDirection());
			//System.out.println("Player: " + index + " isAttacking: " + GameData.getGameData().getPlayer(index).isAttacking());
		}
		for (GameData.ActivePowerup p : GameData.getGameData().getActivePowerups()) {
			//System.out.println("powerup x:" + p.getX_pos() + " y:" +  p.getY_pos() + " weapontype:"+ p.getWeaponType());
		}
		SceneManager.getManager().update(container, delta);
		
		
	}
	
    public void render(GameContainer container, Graphics g) throws SlickException {
    	SceneManager.getManager().render(container, g);
    }
   
    
}