﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.IO;

namespace MoFlowMeshToolset.Classes
{
    /// <summary>
    /// struct for storing all the options for converting to moanim
    /// </summary>
    public struct MoAnimOptions
    {
        public bool mbSwapYAndZ;
        public bool mbMirrorInXZPlane;
    }
    public class CMoAnimConverter
    {
        //constants
        const String kstrPathToMoAnimTool = "ColladaToMoAnimConverter.jar";
        const String kstrPathOutput = "Output/Animations/";

        //private data
        MainWindow mMainWindow;
        bool mbTaskActive;
        MoAnimOptions mOptions;
        string[] mastrFilenames;
        string mstrCurrentStandardError;

        /// <summary>
        /// Constructor
        /// </summary>
        public CMoAnimConverter(MainWindow inParent)
        {
            mMainWindow = inParent;
            mbTaskActive = false;
        }


        /// <summary>
        /// Adds a line of text to the output window. This can be called
        /// from any thread.
        /// </summary>
        /// <param name="instrText"></param>
        public void WriteLineToOutput(string instrText)
        {
            mMainWindow.GetMainThreadDispatcher().BeginInvoke(new MainWindow.AddTextDelegate(mMainWindow.WriteLineToOutput), instrText);
        }

        /// <summary>
        /// Event called when there is new data from the processes std out
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ReadStandardOutputEvent(object sender, DataReceivedEventArgs e)
        {
            WriteLineToOutput(e.Data);
        }

        /// <summary>
        /// event received when there is new data from the process std error
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ReadStandardErrorEvent(object sender, DataReceivedEventArgs e)
        {
            mstrCurrentStandardError += e.Data + "\n";
        }

        /// <summary>
        /// Takes each of the passed in files and runs them through the ColladaToMoStaticConverter, with the user selected options.
        /// </summary>
        /// <param name="inastrFilenames"></param>
        public void ConvertToMoAnim(string[] inastrFilenames, MoAnimOptions inOptions)
        {
            if (mbTaskActive == false)
            {
                //start progress bar
                mMainWindow.StartProgressBar((double)inastrFilenames.Length);

                //clear the current output window
                mMainWindow.ClearOutputWindow();

                //take a copy of the data to be used for conversion
                mOptions = inOptions;
                mastrFilenames = new string[inastrFilenames.Length];
                inastrFilenames.CopyTo(mastrFilenames, 0);

                //start the task and flag the background task as running.
                mbTaskActive = true;
                ThreadPool.QueueUserWorkItem(this.MoAnimConversionTask);
            }
        }

        /// <summary>
        /// Task for loading a task in the background.
        /// </summary>
        public void MoAnimConversionTask(object o)
        {
            List<KeyValuePair<string, string>> ErrorLog = new List<KeyValuePair<string, string>>();

            //iterate through each file
            foreach (string strFilepath in mastrFilenames)
            {
                WriteLineToOutput("Converting '" + mastrFilenames[0] + "':");

                //get the output name
                string[] strFilepathBrokenUp = strFilepath.Split('/');
                string strWrongExtension = strFilepathBrokenUp[strFilepathBrokenUp.Length - 1];
                string strOutputName = strWrongExtension.Replace(".dae", "");
                strOutputName = strOutputName.Replace(".DAE", "");

                //Create the output directory
                Directory.CreateDirectory(kstrPathOutput + "/");

                //create and start the process
                Process process = new Process();
                process.StartInfo.CreateNoWindow = true;
                process.StartInfo.FileName = "java.exe";
                process.StartInfo.Arguments = "-jar " + kstrPathToMoAnimTool
                    + " --input \"" + strFilepath + "\""
                    + " --output \"" + kstrPathOutput + strOutputName + ".moanim\""
                    + " --transforms " + GetModificationsString(mOptions)
                    + " --logginglevel verbose";

                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.RedirectStandardError = true;
                process.StartInfo.ErrorDialog = true;
                process.Start();

                //start asyc read all standard output and print it to the output window.
                mstrCurrentStandardError = "";
                process.OutputDataReceived += this.ReadStandardOutputEvent;
                process.ErrorDataReceived += this.ReadStandardErrorEvent;
                process.BeginErrorReadLine();
                process.BeginOutputReadLine();

                //wait for this process to finish
                process.WaitForExit();

                //remove the events
                process.OutputDataReceived -= this.ReadStandardOutputEvent;
                process.ErrorDataReceived -= this.ReadStandardErrorEvent;

                //store error output
                ErrorLog.Add(new KeyValuePair<string, string>(strOutputName, mstrCurrentStandardError));
                mstrCurrentStandardError = "";

                //incriment the progress bar
                mMainWindow.GetMainThreadDispatcher().BeginInvoke(new MainWindow.ProgressDelegate(mMainWindow.IncrimentProgressBar));
            }

            bool bErrorFound = false;
            //write the error output
            foreach (KeyValuePair<string, string> kvp in ErrorLog)
            {
                if (kvp.Value.Length > 2)
                {
                    WriteLineToOutput(kvp.Key + ":\n" + kvp.Value);
                    //WriteLineToOutput();
                    bErrorFound = true;
                }
            }

            if (bErrorFound == false)
            {
                WriteLineToOutput("All Files successfully converted!");
            }

            //finish the progress bar
            mMainWindow.GetMainThreadDispatcher().BeginInvoke(new MainWindow.ProgressDelegate(mMainWindow.EndProgressBar));

            //clean up and reset to allow the task to be started again.
            mastrFilenames = null;
            mbTaskActive = false;
        }

        /// <summary>
        /// Returns the "features" string  for passing to the MoModel converter.
        /// </summary>
        /// <param name="inOptions"></param>
        /// <returns></returns>
        private string GetFeaturesString(MoAnimOptions inOptions)
        {
            string output = "";
            
            //no features yet...

            if (output.Length > 0 && output[output.Length - 1] == ',')
                output = output.Substring(0, output.Length - 1);

            if (output.Length == 0)
                output = "none";

            return output;
        }


        /// <summary>
        /// Returns the "modifications" string for passing to the MoModel converter.
        /// </summary>
        /// <param name="inOptions"></param>
        /// <returns></returns>
        private string GetModificationsString(MoAnimOptions inOptions)
        {
            string output = "";

            if (inOptions.mbSwapYAndZ == true)
                output += "y";
            if (inOptions.mbMirrorInXZPlane == true)
                output += "m";

            if (output.Length == 0)
                output = "0";

            return output;
        }
    }
}
