﻿using System.Diagnostics;
using System.Globalization;
using System.Windows.Forms;

namespace ModManagerCommon.Controls
{
	public partial class ModUpdateDetails : UserControl
	{
		public ModUpdateDetails()
		{
			InitializeComponent();
			SetData(null);
		}

		public void SetData(ModDownload entry)
		{
			if (entry == null)
			{
				// Download details
				labelDownloadPublished.Text = null;
				labelSize.Text              = null;
				labelFileCount.Text         = null;

				// Release details
				labelReleasePublished.Text = null;
				linkRelease.Text           = null;
				labelReleaseName.Text      = null;
				labelReleaseTag.Text       = null;
			}
			else
			{
				// Download details
				labelDownloadPublished.Text = entry.Updated.ToString(CultureInfo.CurrentCulture);
				labelSize.Text              = SizeSuffix.GetSizeSuffix(entry.Size);
				labelFileCount.Text         = entry.FilesToDownload.ToString();

				// Release details
				labelReleasePublished.Text = entry.Published.ToString(CultureInfo.CurrentCulture);
				linkRelease.Text           = entry.ReleaseUrl;
				labelReleaseName.Text      = entry.Name;
				labelReleaseTag.Text       = entry.Version;
			}

			linkRelease.Enabled = !string.IsNullOrEmpty(linkRelease.Text);
			Enabled = entry != null;
		}

		private void linkRelease_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			Process.Start(linkRelease.Text);
		}
	}
}
