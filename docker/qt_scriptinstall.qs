function Controller() {
  installer.autoRejectMessageBoxes();
}

Controller.prototype.WelcomePageCallback = function() {
  gui.clickButton(buttons.NextButton, 3000);
}

Controller.prototype.CredentialsPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
  var targetDir = installer.environmentVariable("TRIK_QT_INSTALL_DIR");
  if (targetDir == "") {
    targetDir = installer.value("HomeDir") + "/Qt";
  }
  gui.currentPageWidget().TargetDirectoryLineEdit.setText(targetDir);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
  gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
  var widget = gui.currentPageWidget();
  var targetPlatform = {"darwin":"clang_64", "winnt":"undefined", "linux":"gcc_64"}[systemInfo.kernelType];
  widget.deselectAll();
  widget.selectComponent("qt.qt5.5124."+targetPlatform);
  widget.selectComponent("qt.qt5.5124.qtscript");
  widget.selectComponent("qt.tools.ifw.31");

  installer.calculateComponentsToInstall();


  var components = installer.components();
  for (var i = 0 ; i < components.length ;i++) {
    var c = components[i];
    var n = c.name;
    var examplesId = "qt.qt5.5124.examples";
    var docId = "qt.qt5.5124.doc";
    if (n.startsWith(examplesId) || n.startsWith(docId)) {
      widget.deselectComponent(n);
      print("Deselecting:" +  n);
    }
  }
  widget.deselectComponent(examplesId);
  widget.deselectComponent(docId);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
  gui.clickButton(buttons.CommitButton);
  installer.installationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });
}

Controller.prototype.FinishedPageCallback = function() {
  var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm;
  if (checkBoxForm) {
    checkBoxForm.launchQtCreatorCheckBox.setChecked(false);
  }
  else {
    gui.currentPageWidget().RunItCheckBox.setChecked(false);
  }
  gui.clickButton(buttons.FinishButton);
}
