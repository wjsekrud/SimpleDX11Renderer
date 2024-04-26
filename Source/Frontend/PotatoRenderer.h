#pragma once

namespace Frontend {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm에 대한 요약입니다.
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	protected:
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Button^ LoadModelBtn;
	private: System::Windows::Forms::TextBox^ FileLocation;


	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::TextBox^ LightXCoord;
	private: System::Windows::Forms::TextBox^ LightYCoord;



	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::TextBox^ LightZCoord;

	private: System::Windows::Forms::Button^ AddLightBtn;

	private: System::Windows::Forms::TreeView^ treeView1;


	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;
		
#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다. 
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
		/// </summary>
		void InitializeComponent(void)
		{
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->LoadModelBtn = (gcnew System::Windows::Forms::Button());
			this->FileLocation = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->LightXCoord = (gcnew System::Windows::Forms::TextBox());
			this->LightYCoord = (gcnew System::Windows::Forms::TextBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->LightZCoord = (gcnew System::Windows::Forms::TextBox());
			this->AddLightBtn = (gcnew System::Windows::Forms::Button());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(12, 12);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(1080, 757);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"굴림", 14));
			this->label1->Location = System::Drawing::Point(12, 791);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(132, 24);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Load Model";
			this->label1->Click += gcnew System::EventHandler(this, &MyForm::label1_Click);
			// 
			// LoadModelBtn
			// 
			this->LoadModelBtn->Location = System::Drawing::Point(802, 787);
			this->LoadModelBtn->Name = L"LoadModelBtn";
			this->LoadModelBtn->Size = System::Drawing::Size(139, 34);
			this->LoadModelBtn->TabIndex = 2;
			this->LoadModelBtn->Text = L"Load";
			this->LoadModelBtn->UseVisualStyleBackColor = true;
			// 
			// FileLocation
			// 
			this->FileLocation->BackColor = System::Drawing::SystemColors::Menu;
			this->FileLocation->ForeColor = System::Drawing::SystemColors::GrayText;
			this->FileLocation->Location = System::Drawing::Point(150, 791);
			this->FileLocation->Name = L"FileLocation";
			this->FileLocation->Size = System::Drawing::Size(646, 25);
			this->FileLocation->TabIndex = 3;
			this->FileLocation->Text = L"Type File Location";
			this->FileLocation->TextChanged += gcnew System::EventHandler(this, &MyForm::FileLocation_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"굴림", 14));
			this->label2->Location = System::Drawing::Point(12, 836);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(108, 24);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Add Light";
			// 
			// LightXCoord
			// 
			this->LightXCoord->ForeColor = System::Drawing::SystemColors::GrayText;
			this->LightXCoord->Location = System::Drawing::Point(150, 835);
			this->LightXCoord->Name = L"LightXCoord";
			this->LightXCoord->Size = System::Drawing::Size(100, 25);
			this->LightXCoord->TabIndex = 5;
			this->LightXCoord->Text = L"X:";
			this->LightXCoord->TextChanged += gcnew System::EventHandler(this, &MyForm::textBox1_TextChanged_1);
			// 
			// LightYCoord
			// 
			this->LightYCoord->ForeColor = System::Drawing::SystemColors::GrayText;
			this->LightYCoord->Location = System::Drawing::Point(256, 835);
			this->LightYCoord->Name = L"LightYCoord";
			this->LightYCoord->Size = System::Drawing::Size(100, 25);
			this->LightYCoord->TabIndex = 6;
			this->LightYCoord->Text = L"Y:";
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(468, 837);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(87, 19);
			this->checkBox1->TabIndex = 8;
			this->checkBox1->Text = L"Orbiting\?";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// LightZCoord
			// 
			this->LightZCoord->ForeColor = System::Drawing::SystemColors::GrayText;
			this->LightZCoord->Location = System::Drawing::Point(362, 835);
			this->LightZCoord->Name = L"LightZCoord";
			this->LightZCoord->Size = System::Drawing::Size(100, 25);
			this->LightZCoord->TabIndex = 7;
			this->LightZCoord->Text = L"Z:";
			// 
			// AddLightBtn
			// 
			this->AddLightBtn->Location = System::Drawing::Point(561, 828);
			this->AddLightBtn->Name = L"AddLightBtn";
			this->AddLightBtn->Size = System::Drawing::Size(139, 34);
			this->AddLightBtn->TabIndex = 9;
			this->AddLightBtn->Text = L"Add";
			this->AddLightBtn->UseVisualStyleBackColor = true;
			// 
			// treeView1
			// 
			this->treeView1->Location = System::Drawing::Point(1098, 12);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(300, 757);
			this->treeView1->TabIndex = 10;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1410, 1062);
			this->Controls->Add(this->treeView1);
			this->Controls->Add(this->AddLightBtn);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->LightZCoord);
			this->Controls->Add(this->LightYCoord);
			this->Controls->Add(this->LightXCoord);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->FileLocation);
			this->Controls->Add(this->LoadModelBtn);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"MyForm";
			this->Text = L"PotatoRenderer 0.1";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void FileLocation_TextChanged(System::Object^ sender, System::EventArgs^ e) {

	}
private: System::Void textBox1_TextChanged_1(System::Object^ sender, System::EventArgs^ e) {
}
};
}
