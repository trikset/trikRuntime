# Adding Translation to the TRIK firmware (trik-runtime) Project

1. Fork the repository:
  * Go to the [project repository page](https://github.com/trikset/trikRuntime/tree/master) on GitHub.
  * Click the "Fork" button in the top right corner. This will create a copy of the repository on your account.

2. Clone your forked repository to your computer (as always, with Github Desktop client or from command-line):
```shell
    git clone https://github.com/your-username/trikRuntime.git
    cd trikRuntime
```

3. Create a new branch for your work:
```shell
    git checkout -b add-translation-<language_code>
```

4. Edit the translation (more details below).

5. Run the project (**Qt Linguist** is recommended) and ensure that the new language is displayed correctly.

6. Commit changes and push them to GitHub:
```shell
    git add .
    git commit -m "Add translation for <language>"
    git push origin add-translation-<language_code>
```

7. Create a Pull Request:
  * Go to your forked repository on GitHub.
  * Click the "Compare & pull request" button next to your branch.
  * Fill in the description of the changes and create the Pull Request. Other contributors may review and request changes if necessary. Once the changes are reviewed and approved, they can be merged into the main repository.


## Adding Translation for a New Language
  1. Add a translation file for the desired language to the `translations` folder in the format `trikRuntime_<language_code>.ts`.
  2. Copy the structure from the `trikRuntime_sample.ts` file. Replace `language="en"` with the code of your language (check it [here](https://en.wikipedia.org/wiki/List_of_ISO_639_language_codes) --- Table, Set 1).
  3. Modify the project code to use translations from the new file. Open the `translations/translations.pro` file in **Qt Creator** and add a line after `TRANSLATIONS +=` with the name of your file:
```shell
        $$PWD/trikRuntime_<language_code>.ts \ 
```
  4. Open the `trikRuntime.pro` file and add a line after `TRANSLATIONS +=` with the name of your file:
```shell
        $$PWD/translations/trikRuntime_<language_code>.ts \
```
  5. Run lupdate in **Qt Creator**: go to *Tools -> External -> Linguist -> Update Translations (lupdate)*. Now your created file contains the source strings for translation.
  6. Add translations for the language using Qt Linguist. Open your file through **Qt Linguist** and add translations to the strings.
  7. Rebuild the project in **Qt Creator** (*Build -> Build Project trikRuntime*) to generate new translation files.

## Editing Existing Translation Files
  1. It is recommended to use **Qt Linguist**. Open your file through it and add translations to the strings.
  2. Rebuild the project in **Qt Creator** (*Build -> Build Project trikRuntime*) to generate new translation files.
