Public Class Form1
    Private Sub ToolStrip1_ItemClicked(sender As Object, e As ToolStripItemClickedEventArgs)

    End Sub

    Private Sub ToolStripButton2_Click(sender As Object, e As EventArgs)

    End Sub

    Private Sub SerialSettingsToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles SerialSettingsToolStripMenuItem.Click
        Dialog_SerialSettings.ShowDialog()
    End Sub
End Class
