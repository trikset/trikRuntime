function Controller() {
  installer.autoRejectMessageBoxes();
  installer.installationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });
  installer.setMessageBoxAutomaticAnswer("cancelInstallation", QMessageBox.Yes);
}

Controller.prototype.WelcomePageCallback = function() {
  gui.clickButton(buttons.NextButton, 3000);
  gui.clickButton(buttons.NextButton, 3000);
}

Controller.prototype.CredentialsPageCallback = function() {
    var widget = gui.currentPageWidget();
    widget.loginWidget.EmailLineEdit.setText("");
    widget.loginWidget.PasswordLineEdit.setText("");
    gui.clickButton(buttons.NextButton, 500);
}

Controller.prototype.IntroductionPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.DynamicTelemetryPluginFormCallback = function() {
  var page = gui.pageWidgetByObjectName("DynamicTelemetryPluginForm");
  page.statisticGroupBox.disableStatisticRadioButton.setChecked(true);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
  var widget = gui.currentPageWidget()
  if (widget != null) {
    var targetDir = installer.environmentVariable("TRIK_QT_INSTALL_DIR");
    if (targetDir == "") {
      targetDir = installer.value("HomeDir") + "/Qt";
    }
    widget.TargetDirectoryLineEdit.setText(targetDir);
  }
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
  var qtVersionId = "qt.qt5.5125."
  widget.selectComponent(qtVersionId + targetPlatform);
  widget.selectComponent(qtVersionId + "qtscript");
  widget.selectComponent("qt.tools.ifw.31");

  installer.calculateComponentsToInstall();


  var components = installer.components();
  for (var i = 0 ; i < components.length ;i++) {
    var c = components[i];
    var n = c.name;
    var examplesId = qtVersionId + "examples";
    var docId = qtVersionId + "doc";
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
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.PerformInstallationPageCallback = function() {
  gui.clickButton(buttons.CommitButton);
}

Controller.prototype.FinishedPageCallback = function() {
  var page = gui.currentPageWidget();
  var checkBoxForm = page.LaunchQtCreatorCheckBoxForm;
  if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
    checkBoxForm.launchQtCreatorCheckBox.setChecked(false);
  }
  if (page.RunItCheckBox) {
    page.RunItCheckBox.setChecked(false);
  }
  gui.clickButton(buttons.FinishButton);
}
