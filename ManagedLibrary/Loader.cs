namespace ManagedLibrary;

public class Loader
{
    public delegate void EntryDelegate();

    public static void Entry()
    {
        for (int i = 0; i < 5; i++)
        {
            System.Diagnostics.Debug.WriteLine($"Hello World; From hosted .NET! {i}");
        }
    }
}
