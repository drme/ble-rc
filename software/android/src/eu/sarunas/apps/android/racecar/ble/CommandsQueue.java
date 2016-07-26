package eu.sarunas.apps.android.racecar.ble;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class CommandsQueue
{
	public CommandsQueue(String name)
	{
		this.executorThread = new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				while (CommandsQueue.this.running)
				{
					checkBlock();

					if (runNextCommand())
					{
						//CommandsQueue.sleep(10);
					}
					else
					{
						CommandsQueue.sleep(100);
					}
				}
			};
		});

		this.executorThread.setName(name);
		this.executorThread.start();
	};

	public synchronized void add(Runnable command)
	{
		this.queue.add(command);
	};

	public synchronized void add(Runnable commandRequest, Runnable commandStatus)
	{
		this.queue.add(commandRequest);
		this.queue.add(commandStatus);
	};
	
	public void stop()
	{
		this.queue.clear();
		this.running = false;
	};

	private boolean runNextCommand()
	{
		Runnable command = this.queue.poll();

		if (command != null)
		{
			try
			{
				command.run();
			}
			catch (Throwable ex)
			{
				ex.printStackTrace();
			}

			return true;
		}
		else
		{
			return false;
		}
	};

	public static void sleep(long time)
	{
		try
		{
			Thread.sleep(time);
		}
		catch (InterruptedException e)
		{
			e.printStackTrace();
		}
	};

	private void checkBlock()
	{
		while (true == this.blockCommands)
		{
			CommandsQueue.sleep(10);
		}
	};

	public void setBlockCommands(boolean block)
	{
		this.blockCommands = block;
	};

	public void clear()
	{
		this.queue.clear();
	};
	
	private boolean blockCommands = true;
	private boolean running = true;
	private Thread executorThread;
	private Queue<Runnable> queue = new ConcurrentLinkedQueue<Runnable>();
};
