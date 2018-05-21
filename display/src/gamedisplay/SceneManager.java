package gamedisplay;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.SlickException;


/**
 * 
 * @author Florian https://www.youtube.com/playlist?list=PLfn8QHAlkClCRNo1LNmA9UDns3-y77uLC
 * The SceneManager,
 * it controlls and manages all aktiv scenes and gives us some more comfort later on
 * its a singleton object ->   you can access it everywhere with : SceneManager.getManager().yourMethodeCall()
 */
public class SceneManager 
{
	// A list with all our scenes
	private List<Scene> scenes ;
	// The Gamecontainer to handle the init method
	private GameContainer gc;
	//A manager Object -> Singelton
	private static SceneManager manager;
	
	public SceneManager ( GameContainer gc )
	{
		this.gc = gc;
		scenes = new ArrayList<Scene>();
	}
	
	// Singelton method
	public static SceneManager getManager ()
	{
		if( manager == null ) manager = new SceneManager(null);
		return manager;
	}
	
	// Set GameContainer
	public void setGameContainer( GameContainer gc )
	{
		this.gc = gc;
	}
	
	// Add a scene to the list and call the init method
	public void addScene ( Scene scene )
	{
		scenes.add( scene );
		try {
			scene.init(gc);
		} catch (SlickException e) {
			e.printStackTrace();
		}
		Collections.sort(scenes);
	}
	
	// Removes a scene by an specific object
	public void removeScene ( Scene scene )
	{
		scenes.remove(scene);
	}
	
	// Removes a scene by an specific name
	public boolean removeScene ( String scene )
	{
		for( int i = 0 ; i < scenes.size() ; i++ )
		{
			if( scenes.get(i).toString().equals(scene) )
			{
				scenes.remove(i);
				return true;
			}
		}
		return false;
	}
	
	// Render all scenes
	public void render(GameContainer gc, Graphics g) throws SlickException {
		for( int i = 0 ; i < scenes.size() ; i++ )
		{
			scenes.get(i).render(gc, g);
		}
	}
	
	// Update all scenes
	public void update(GameContainer gc, int t) throws SlickException {
		for( int i = 0 ; i < scenes.size() ; i++ )
		{
			scenes.get(i).update(gc,t);
		}
	}
	
	// Get a scene by name
	public Scene getScene ( String scene )
	{
		for( int i = 0 ; i < scenes.size() ; i++ )
		{
			if( scenes.get(i).toString().equals(scene) )
			{
				return scenes.get(i);
			}
		}
		return null;
	}
	
	// Re-Sort the list 
	public void sort ()
	{
		Collections.sort(scenes);
	}
	
	// Clear the list
	public void clear ()
	{
		scenes = new ArrayList<Scene>();
	}
}
