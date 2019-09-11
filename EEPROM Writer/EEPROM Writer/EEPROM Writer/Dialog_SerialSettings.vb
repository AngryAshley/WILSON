Imports System.Windows.Forms

Public Class Dialog_SerialSettings

    Dim connectionCount As Integer = 0
    Dim COM As IO.Ports.SerialPort = Nothing
    Dim connected As Boolean = False
    Dim response As String = ""

    Private Sub OK_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OK_Button.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub

    Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub

    Private Sub Dialog_SerialSettings_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        For Each sp As String In My.Computer.Ports.SerialPortNames
            ListBox1.Items.Add(sp)
            connectionCount += 1
        Next

        If ListBox1.Items.Count = 0 Then
            ListBox1.Items.Add("No COM port available")
            ListBox1.Enabled = False
        Else
            ListBox1.SelectedIndex = 0
        End If

    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        If connectionCount = 0 Then
            Dim X As DialogResult = MessageBox.Show("No COM port available", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error)

            ListBox1.Enabled = False

        Else
            Label_connectionStatus.ForeColor = Color.Black
            Label_connectionStatus.Text = "Connecting..."
            Try
                COM = My.Computer.Ports.OpenSerialPort(ListBox1.SelectedItem)
                COM.ReadTimeout = 1000

            Catch ex As TimeoutException
                Label_connectionStatus.BackColor = Color.Red
                Label_connectionStatus.Text = "Error: Serial Port timed out."
            Finally

            End Try
            Label_connectionStatus.ForeColor = Color.Black
            Label_connectionStatus.Text = "Port opened, handshaking"

            For index As Integer = 0 To 5
                COM.Write("HANDSHAKE")
                COM.Write(New Byte() {&HD}, 0, 1)
                COM.Write(New Byte() {&HA}, 0, 1)
                Try
                    response = COM.ReadLine()
                Catch ex As Exception

                End Try

                If response Is "HANDSHAKE" Then
                    Exit For
                    connected = True
                End If
                Label_connectionStatus.Text += "."
            Next

            If Not connected Then
                Label_connectionStatus.ForeColor = Color.Red
                Label_connectionStatus.Text = "Error: Handshake failed"
                COM.Close()
            End If


        End If
    End Sub
End Class
