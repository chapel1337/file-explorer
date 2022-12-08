#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <conio.h>
#include <deque>
#include <thread>
#include <chrono>
#include <windows.h>

using std::cout, std::cin, std::string, std::deque, std::this_thread::sleep_for, std::chrono::seconds, std::getline, std::ws, std::ofstream;
using namespace std::filesystem;

/* 
* written by chapel1337
* started on 12/5/2022
* finished on 12/7/2022
* got consistently distracted on 11/6/2022; wasn't able to get much done
* duplicate file name pasting was hell, microsoft really likes to give vague error messages
* i finally learnt the filesystem library!!
* not much else to say about this, it's just a simple file explorer
* trying to fix these filesystem errors is less painful than shooting my dominant eye with a barrett; HOW ARE MICROSOFT DEVELOPERS THIS INCOMPETENT?!?!? JUST GIVE ME THE DAMN ERROR, IS IT REALLY THAT HARD????
*/

deque<path> directoryContents{};
deque<int> discludedPaths{};

short position{};

bool searching{};
string searchString{};

string newName{};
path clipboard{};

void fileConfirm(const short actionOption, const short optionPosition);
void fileSearch();

void clear()
{
	system("cls");
}

void title(const string& title)
{
	SetConsoleTitleA(("files explorer - " + title).c_str());
}

void displayFileStats()
{
	cout << "parent directory:	" << directoryContents[position].parent_path().string() << '\n';

	cout << "file name:		";
	if (directoryContents[position].stem().string().length() + directoryContents[position].extension().string().length() > 40)
	{
		cout << directoryContents[position].stem().string().substr(0, 40) << directoryContents[position].extension().string() << " ...\n";
	}
	else
	{
		cout << directoryContents[position].stem().string() << directoryContents[position].extension().string() << '\n';
	}

	cout << "file size:		";
	if (file_size(directoryContents[position]) > 1000)
	{
		cout << file_size(directoryContents[position]) / 1000 << " (kilobytes)\n\n";
	}
	else if (file_size(directoryContents[position]) > 100)
	{
		cout << file_size(directoryContents[position]) << " (bytes)\n\n";
	}
	else if (file_size(directoryContents[position]) <= 100)
	{
		cout << "0 (bytes)\n\n";
	}
}

void displayStats()
{
	cout << "clipboard:		" << clipboard.string() << "\n\n";
	cout << "current path:		" << directoryContents[0].parent_path().string() << '\n';
	cout << "directory file amount:  " << directoryContents.size() - 1 << "\n\n";
}

// i love spending 25 minutes trying to figure out what is breaking just to fix it by removing the reference
// i hope you have a bad christmas bill gates
void setDirectory(const path newDirectory)
{
	try 
	{
		position = 0;
		directoryContents.clear();

		for (const auto& file : directory_iterator(newDirectory))
		{
			directoryContents.push_back(file.path());
		}
		directoryContents.push_front(newDirectory / "..");
	}
	catch (filesystem_error& test)
	{
		cout << test.what();
	}
}

void help()
{
	clear();

	cout << "help\n\n";

	cout << "this functions like your typical file explorer\n\n";

	cout << "the pointer to the right of a file indicates that the file is selected\n";
	cout << "use the w and d keys to move it up and down\n";
	cout << "press the enter key to view view selected directories\n";
	cout << "select the '../' file to go to the parent directory\n\n";
	
	cout << "to filter file names, press f and type; when done, press esc\n";
	cout << "to rename a file, press r and type in the new name (to exit the process, press esc)\n";
	cout << "to delete a file, press d and confirm the deletion (to exit the process, press esc)\n";
	cout << "to copy a file, press c\n";
	cout << "to paste a file, press v (you are able to paste the same file up to 9 times)\n\n";

	cout << "written by chapel1337\n";
	cout << "started on 12/5/2022\n";
	cout << "finished on 12/7/2022\n\n";

	cout << "okay	<\n";
	_getch();
}

void refresh()
{
	clear();
	displayStats();
	title(directoryContents[0].parent_path().string());

	if (searching)
	{
		cout << "> " << searchString << " <\n\n";

		discludedPaths.clear();
		for (unsigned int i{}; i < directoryContents.size(); ++i)
		{
			if (directoryContents[i].stem().string().find(searchString) == string::npos && directoryContents[i].extension().string().find(searchString) == string::npos)
			{
				// cout << "no: " << directoryContents[i].string() << '\n';
				discludedPaths.push_back(1);
			}
			else
			{
				// cout << "yes: " << directoryContents[i].string() << '\n';
				discludedPaths.push_back(0);
			}
		}

		cout << "\n\n";
	}

	for (unsigned int i{}; i < directoryContents.size(); ++i)
	{
		if (searching && discludedPaths[i] == 1)
		{
			continue;
		}

		if (i == position && directoryContents[i].string().rfind('\\') != string::npos)
		{
			if (directoryContents[i].stem().string().length() + directoryContents[i].extension().string().length() > 40)
			{
				cout << directoryContents[i].stem().string().substr(0, 40) << directoryContents[i].extension().string() << " ...";
			}
			else
			{
				// cout << directoryContents[i].string().substr(directoryContents[i].string().rfind('\\') + 1, directoryContents[i].string().length());
				cout << directoryContents[i].filename().string();
			}

			if (is_directory(directoryContents[i]))
			{
				cout << '\\';
			}
			cout << "	<\n";
		}
		else if (directoryContents[i].string().rfind('\\') != string::npos)
		{
			if (directoryContents[i].stem().string().length() + directoryContents[i].extension().string().length() > 40)
			{
				cout << directoryContents[i].stem().string().substr(0, 40) << directoryContents[i].extension().string() << " ...";
			}
			else
			{
				cout << directoryContents[i].filename().string();
			}

			if (is_directory(directoryContents[i]))
			{
				cout << '\\';
			}
			cout << '\n';
		}

		/*
		else if (i == position && directoryContents[i].string().rfind('\\') == string::npos)
		{
			cout << directoryContents[i].string() << "	<\n";
		}
		else if (directoryContents[i].string().rfind('\\') == string::npos)
		{
			cout << directoryContents[i].string() << '\n';
		}
		
		cout << "			";

		if (file_size(directoryContents[i]) > 1000)
		{
			cout << file_size(directoryContents[i]) / 1000 << " (kilobytes)\n";
		}
		else if (file_size(directoryContents[i]) > 100)
		{
			cout << file_size(directoryContents[i]) << " (bytes)\n";
		}
		else if (file_size(directoryContents[i]) <= 100)
		{
			cout << "0 (bytes)\n";
		}
		*/
	}
}

void fileSearch()
{
	if (!searching)
	{
		position = -1;
		searching = true;
	}

	const int keyInput{ _getch() };

	if (keyInput == 27)
	{
		searching = false;
		position = 0;

		searchString.clear();
		return;
	}
	else if (keyInput == 8 && searchString.length() != 0)
	{
		searchString.pop_back();
	}
	else if (keyInput <= 31 || keyInput == 127)
	{
		fileSearch();
	}
	else
	{
		searchString.push_back(static_cast<char>(keyInput));
	}

	refresh();
	fileSearch();
}

void getInput()
{
	const int keyInput{ _getch() };
	
	if ((keyInput == 'W' || keyInput == 'w') && position != 0)
	{
		--position;
	}
	else if ((keyInput == 'S' || keyInput == 's') && position != directoryContents.size() - 1)
	{
		++position;
	}
	else if (keyInput == ' ' || keyInput == 13)
	{
		if (is_directory(directoryContents[position]) && position != 0)
		{
			setDirectory(directoryContents[position]);
		}
		else if (is_directory(directoryContents[position]) && position == 0)
		{
			// setDirectory(static_cast<path>(directoryContents[position].string().substr(directoryContents[position].string().rfind('\\') - 1, directoryContents[position].string().length())));

			setDirectory(directoryContents[0].parent_path().parent_path());
		}
	}
	else if (keyInput == 'H' || keyInput == 'h')
	{
		title("help");
		help();
	}
	else if (keyInput == 'D' || keyInput == 'd')
	{
		title("delete file");
		fileConfirm(0, 0);
	}
	else if (keyInput == 'F' || keyInput == 'f')
	{
		title("filter files");
		fileSearch();
	}
	else if (keyInput == 'C' || keyInput == 'c')
	{
		clipboard = directoryContents[position];
	}
	else if (keyInput == 'V' || keyInput == 'v')
	{
		// this took an absurdly long time to make work
		
		// ofstream output{ directoryContents[0].parent_path().string() + '\\' + (clipboard.stem().string() + clipboard.extension().string()) };
		
		int tries{ 1 };
		int mismatches{};

		while (true)
		{
			if (tries == 10)
			{
				break;
			}

			for (int i{}; i < directoryContents.size(); ++i)
			{
				if (directoryContents[i].filename().string() != clipboard.stem().string() + " - " + char('0' + tries) + clipboard.extension().string())
				{
					++mismatches;
				}
			}

			if (mismatches == directoryContents.size())
			{
				break;
			}
			mismatches = 0;

			++tries;
		}

		if (tries == 0)
		{
			copy(clipboard, directoryContents[0].parent_path(), copy_options::recursive);
		}
		else if (tries != 10)
		{
			copy(clipboard, directoryContents[0].parent_path() / path(clipboard.stem().string() + " - " + char('0' + tries) + clipboard.extension().string()), copy_options::recursive);
		}

		setDirectory(directoryContents[0].parent_path());
	}
	else if (keyInput == 'R' || keyInput == 'r')
	{
		clear();
		title("rename file");
		displayFileStats();
		
		cout << "what would you like to rename this file to?\n\n";

		cout << "> ";
		getline(cin >> ws, newName);

		fileConfirm(1, 0);
	}

	refresh();
	getInput();
}

// delete = 0, rename = 1
void fileConfirm(const short actionOption, const short optionPosition)
{
	clear();
	displayFileStats();

	if (actionOption == 0)
	{
		cout << "are you sure you want to delete this file?\n\n";
	}
	else if (actionOption == 1)
	{
		cout << "are you sure you want to rename this file?\n\n";
	}

	if (optionPosition == 0)
	{
		cout << "yes	<\n";
	}
	else
	{
		cout << "yes\n";
	}

	if (optionPosition == 1)
	{
		cout << "no	<\n";
	}
	else
	{
		cout << "no\n";
	}

	const int keyInput{ _getch() };

	if ((keyInput == 'W' || keyInput == 'w') && optionPosition != 0)
	{
		fileConfirm(actionOption, optionPosition - 1);
	}
	else if ((keyInput == 'S' || keyInput == 's') && optionPosition != 1)
	{
		fileConfirm(actionOption, optionPosition + 1);
	}
	else if (keyInput == 27)
	{
		return;
	}
	else if (keyInput == 13 || keyInput == ' ')
	{
		if (optionPosition == 0)
		{
			switch (actionOption)
			{
			case 0:
				remove(directoryContents[position]);
				directoryContents.erase(directoryContents.begin() + position);
				break;

			case 1:
				rename(directoryContents[position], directoryContents[position].parent_path() / path(newName + directoryContents[position].extension().string()));
				break;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		fileConfirm(actionOption, optionPosition);
	}

	setDirectory(directoryContents[0].parent_path());
}

int main()
{
	setDirectory(current_path());
	refresh();
	getInput();
}