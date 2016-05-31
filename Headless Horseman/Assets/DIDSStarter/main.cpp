#define DIRECTINPUT_VERSION 0x800
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>




HINSTANCE main_instance = NULL;
HWND main_hwnd = 0;
bool windowed = true;
D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
LPDIRECTINPUT8 lpDI = NULL;
//Keyboard
LPDIRECTINPUTDEVICE8 lpDIKey = NULL;
UCHAR kb_state[256];
//Mouse
LPDIRECTINPUTDEVICE8 lpDIMouse = NULL;
DIMOUSESTATE m_state;
//Joystick
LPDIRECTINPUTDEVICE8 lpDIJS = NULL;
DIJOYSTATE j_state;
GUID joystickGUID;
char joyname[80];
//Sound
#define MAX_SOUNDS 30
int DSound_Load_WAV(char* filename);


//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int width  = 640;
const int height = 480;

IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9*  IB = 0;
typedef struct pcm_sound_yp
{
	LPDIRECTSOUNDBUFFER dsbuffer;
	int state, rate, size, id;
}pcm_sound, *pcm_sound_ptr;

LPDIRECTSOUND lpds;
DSBUFFERDESC dsbd;
WAVEFORMATEX pcmwf;
pcm_sound sound_fx[MAX_SOUNDS];

int sound_id1 = -1;
int sound_id2 = -1;
int sound_id3 = -1;



//
// Classes and Structures
//
struct Vertex
{	Vertex(){}
	Vertex(float x, float y, float z)
	{
		_x = x;  _y = y;  _z = z;
	}
	float _x, _y, _z;
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
BOOL CALLBACK DI_Enum_Joysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr)
{
	*(GUID*)guid_ptr = lpddi->guidInstance;
	strcpy_s(joyname, (char*)lpddi->tszProductName);
	return(DIENUM_STOP);
}


bool Setup()
{

	//check for DI
	if (FAILED(DirectInput8Create(main_instance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&lpDI, NULL)))
	{
		return 0;
	}
	

// Direct Input 8 - K E Y B O A R D 
	if (FAILED(lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKey, NULL)))
	{
		return 0;
	}
	if (FAILED(lpDIKey->SetCooperativeLevel(main_hwnd,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return 0;
	}
	if (FAILED(lpDIKey->SetDataFormat(&c_dfDIKeyboard)))
	{
		return 0;
	}
	if (FAILED(lpDIKey->Acquire()))
	{
		return 0;
	}
	




// Direct Input 8 - M O U S E 
	if (FAILED(lpDI->CreateDevice(GUID_SysMouse, &lpDIMouse, NULL)))
	{
		return 0;
	}
	if (FAILED(lpDIMouse->SetCooperativeLevel(main_hwnd,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return 0;
	}
	if (FAILED(lpDIMouse->SetDataFormat(&c_dfDIMouse)))
	{
		return 0;
	}
	if (FAILED(lpDIMouse->Acquire()))
	{
		return 0;
	}
// Direct Input 8 - END M O U S E 



// Direct Input 8 - J O Y S T I C K 


	//// first find the GUID of your particular joystick
	lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, DI_Enum_Joysticks, &joystickGUID, DIEDFL_ATTACHEDONLY); 

	if (FAILED(lpDI->CreateDevice(joystickGUID, &lpDIJS, NULL)))
	{
	   return(0);
	}

	if (FAILED(lpDIJS->SetCooperativeLevel(main_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return(0);
	}
	if (FAILED(lpDIJS->SetDataFormat(&c_dfDIJoystick)))
	{
	   return(0);
	}

	// set the range of the joystick
	DIPROPRANGE joy_axis_range;

	// first x axis
	joy_axis_range.lMin = -100;
	joy_axis_range.lMax = 100;

	joy_axis_range.diph.dwSize       = sizeof(DIPROPRANGE); 
	joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joy_axis_range.diph.dwObj        = DIJOFS_X;
	joy_axis_range.diph.dwHow        = DIPH_BYOFFSET;

	lpDIJS->SetProperty(DIPROP_RANGE,&joy_axis_range.diph);

	// now y-axis
	joy_axis_range.lMin = -100;
	joy_axis_range.lMax = 100;

	joy_axis_range.diph.dwSize       = sizeof(DIPROPRANGE); 
	joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joy_axis_range.diph.dwObj        = DIJOFS_Y;
	joy_axis_range.diph.dwHow        = DIPH_BYOFFSET;

	lpDIJS->SetProperty(DIPROP_RANGE,&joy_axis_range.diph);

	// and now the dead band

	DIPROPDWORD dead_band; // here's our property word

	dead_band.diph.dwSize       = sizeof(dead_band);
	dead_band.diph.dwHeaderSize = sizeof(dead_band.diph);
	dead_band.diph.dwObj        = DIJOFS_X;
	dead_band.diph.dwHow        = DIPH_BYOFFSET;

	// 10% will be used on both sides of the range +/-
	dead_band.dwData            = 1000;

	// finally set the property
	lpDIJS->SetProperty(DIPROP_DEADZONE,&dead_band.diph);

	dead_band.diph.dwSize       = sizeof(dead_band);
	dead_band.diph.dwHeaderSize = sizeof(dead_band.diph);
	dead_band.diph.dwObj        = DIJOFS_Y;
	dead_band.diph.dwHow        = DIPH_BYOFFSET;

	// 10% will be used on both sides of the range +/-
	dead_band.dwData            = 1000;

	// finally set the property
	lpDIJS->SetProperty(DIPROP_DEADZONE,&dead_band.diph);


	 //acquire the joystick
	if (FAILED(lpDIJS->Acquire()))
	{
		return(0);
	}

///////////////////////////////////////////////////////////
// Direct Sound Setup
	if (DirectSoundCreate(NULL, &lpds, NULL) != DS_OK)
		return 0;
	if (lpds->SetCooperativeLevel(main_hwnd, DSSCL_NORMAL) != DS_OK)
		return 0;
	//clear out sound array
	memset(sound_fx, 0, sizeof(pcm_sound)*MAX_SOUNDS);
	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		if (sound_fx[i].dsbuffer)
		{
			sound_fx[i].dsbuffer->Stop();
			sound_fx[i].dsbuffer->Release();
		}
		memset(&sound_fx[i], 0, sizeof(pcm_sound));
		//set up new sounds
		sound_fx[i].state = 0;
		sound_fx[i].id = 1;
	}
	//load files
	if ((sound_id1 = DSound_Load_WAV("laugh.wav")) == -1)
	{
		MessageBox(main_hwnd, "laugh Sound Bad", "ERROR", MB_OK);
		return 0;
	}
	if ((sound_id2 = DSound_Load_WAV("flight.wav")) == -1)
	{
		MessageBox(main_hwnd, "flight Sound Bad", "ERROR", MB_OK);
		return 0;
	}
	if ((sound_id3 = DSound_Load_WAV("GAMEOVER.WAV")) == -1)
	{
		MessageBox(main_hwnd, "GAMEOVER Sound Bad", "ERROR", MB_OK);
		return 0;
	}
	sound_fx[sound_id2].dsbuffer->Play(0, 0, DSBPLAY_LOOPING);
	
// Direct Sound
///////////////////////////////////////////////////////////

	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = main_hwnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		main_hwnd,               // window associated with device
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    &Device);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			main_hwnd,
			vp,
			&d3dpp,
			&Device);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object


	//
	// Create vertex and index buffers.
	//

	Device->CreateVertexBuffer(
		8 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Device->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IB,
		0);

	//
	// Fill the buffers with the cube data.
	//

	// define unique vertices:
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);

	// vertices of a unit cube
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f,  1.0f, -1.0f);
	vertices[2] = Vertex( 1.0f,  1.0f, -1.0f);
	vertices[3] = Vertex( 1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f,  1.0f);
	vertices[5] = Vertex(-1.0f,  1.0f,  1.0f);
	vertices[6] = Vertex( 1.0f,  1.0f,  1.0f);
	vertices[7] = Vertex( 1.0f, -1.0f,  1.0f);

	VB->Unlock();

	// define the triangles of the cube:
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	// front side
	indices[0]  = 0; indices[1]  = 1; indices[2]  = 2;
	indices[3]  = 0; indices[4]  = 2; indices[5]  = 3;

	// back side
	indices[6]  = 4; indices[7]  = 6; indices[8]  = 5;
	indices[9]  = 4; indices[10] = 7; indices[11] = 6;

	// left side
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	// right side
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	// top
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	// bottom
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;

	IB->Unlock();

	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

    Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)width / (float)height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//
	// Switch to wireframe mode.
	//

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
// Direct Sound 
	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		if (sound_fx[i].dsbuffer)
			sound_fx[i].dsbuffer->Release();
	}
	if (lpds != NULL)
		lpds->Release();


// Direct Input 8
	if (lpDI)
		lpDI->Release();
	if (lpDIMouse)
		lpDIMouse->Unacquire();
	if (lpDIMouse)
		lpDIMouse->Release();
	if (lpDIKey)
		lpDIKey->Unacquire();
	if (lpDIKey)
		lpDIKey->Unacquire();
	if (lpDIJS)
		lpDIJS->Unacquire();
	if (lpDIJS)
		lpDIJS->Unacquire();
	



	
	if (VB)
		VB->Release();     //Release<IDirect3DVertexBuffer9*>(VB);
	if (IB)
		IB->Release();	   //Release<IDirect3DIndexBuffer9*>(IB);

	
}

bool Display(float timeDelta)
{
	
	if (Device)
	{
		//
		// spin the cube:
		//
		D3DXMATRIX Rx, Ry;
		static float y = 0.0f;
		static float x = 3.14f / 4.0f;

		//// rotate 45 degrees on x-axis
		D3DXMatrixRotationX(&Rx, x);   //3.14159265    6.2831853

		//// incremement y-rotation angle each frame
		D3DXMatrixRotationY(&Ry, y);


		//  K E Y B O A R D
		HRESULT hresult;
		hresult = lpDIKey->GetDeviceState(256, (LPVOID)kb_state);
		if (hresult == DIERR_INPUTLOST)
		{
			if (FAILED(lpDIKey->Acquire()))
			{
				return 0;
			}
		}
		if (kb_state[DIK_RIGHT]&0x80)
		{
			y -= .001f;
			sound_fx[sound_id1].dsbuffer->Play(0, 0, 0);

		}
		if (kb_state[DIK_LEFT] & 0x80)
		{
			y += .001f;
			sound_fx[sound_id2].dsbuffer->Play(0, 0, 0);
		}
		if (kb_state[DIK_UP] & 0x80)
		{
			x -= .001f;
		}
		if (kb_state[DIK_DOWN] & 0x80)
		{
			x += .001f;
		}
		if (kb_state[DIK_R] & 0x80)
		{
			y += 0.0f;
			x = 3.14f / 4.0f;
		}
	

//  END K E Y B O A R D


//  M O U S E
		HRESULT hresult2;
		hresult2 = lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&m_state);
		if (hresult2 == DIERR_INPUTLOST)
		{
			if (FAILED(lpDIMouse->Acquire()))
			{
				return 0;
			}
		}
		x -= (m_state.lY * .01f);
		y -= (m_state.lX * .01f);

//  END M O U S E


//// J O Y S T I C K 
//
		lpDIJS->Poll();
		lpDIJS->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&j_state);
		x -= (j_state.lY * 0.00001f);
		y -= (j_state.lX * 0.00001f);

		if (j_state.rgbButtons[0] & 0x80)
		{
			y = 0.0f;
			x = 3.14f / 4.0f;
		}
		if (j_state.rgbButtons[1] & 0x80)
		{
			y = 30.0f;
			x = 3.14f / 4.0f;
		}
		if (j_state.rgbButtons[2] & 0x80)
		{
			SendMessage(main_hwnd, WM_DESTROY, 0, 0);
		}
	
	
//	// END J O Y S T I C K 





		//// reset angle to zero when angle reaches 2*PI
		if( y >= 6.2831853f )
			y = 0.0f;

		//// combine x- and y-axis rotation transformations.
		D3DXMATRIX p = Rx * Ry;

		Device->SetTransform(D3DTS_WORLD, &p);




		//
		// draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);

		// Draw cube.
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
int DSound_Load_WAV(char *filename)
{
	// this function loads a .wav file, sets up the directsound

	// buffer and loads the data into memory, the function returns

	// the id number of the sound





	HMMIO                hwav;    // handle to wave file

	MMCKINFO             parent,  // parent chunk

		child;   // child chunk

	WAVEFORMATEX    wfmtx;   // wave format structure



	int    sound_id = -1,       // id of sound to be loaded

		index;               // looping variable



	UCHAR *snd_buffer,       // temporary sound buffer to hold voc data

		*audio_ptr_1 = NULL, // data ptr to first write buffer

		*audio_ptr_2 = NULL; // data ptr to second write buffer



	DWORD audio_length_1 = 0,  // length of first write buffer

		audio_length_2 = 0;  // length of second write buffer



							 // step one: are there any open id's ?

	for (index = 0; index < MAX_SOUNDS; index++)

	{

		// make sure this sound is unused

		if (sound_fx[index].state == 0)

		{

			sound_id = index;

			break;

		} // end if



	} // end for index



	  // did we get a free id?

	if (sound_id == -1)

		return(-1);



	// set up chunk info structure

	parent.ckid = (FOURCC)0;

	parent.cksize = 0;

	parent.fccType = (FOURCC)0;

	parent.dwDataOffset = 0;

	parent.dwFlags = 0;



	// copy data

	child = parent;



	// open the WAV file

	if ((hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL)

		return(-1);



	// descend into the RIFF

	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');



	if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))

	{

		// close the file

		mmioClose(hwav, 0);



		// return error, no wave section

		return(-1);

	} // end if



	  // descend to the WAVEfmt

	child.ckid = mmioFOURCC('f', 'm', 't', ' ');



	if (mmioDescend(hwav, &child, &parent, 0))

	{

		// close the file

		mmioClose(hwav, 0);



		// return error, no format section

		return(-1);

	} // end if



	  // now read the wave format information from file

	if (mmioRead(hwav, (char *)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))

	{

		// close file

		mmioClose(hwav, 0);



		// return error, no wave format data

		return(-1);

	} // end if



	  // make sure that the data format is PCM

	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)

	{

		// close the file

		mmioClose(hwav, 0);



		// return error, not the right data format

		return(-1);

	} // end if



	  // now ascend up one level, so we can access data chunk

	if (mmioAscend(hwav, &child, 0))

	{

		// close file

		mmioClose(hwav, 0);



		// return error, couldn't ascend

		return(-1);

	} // end if



	  // descend to the data chunk

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');



	if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))

	{

		// close file

		mmioClose(hwav, 0);



		// return error, no data

		return(-1);

	} // end if



	  // finally!!!! now all we have to do is read the data in and

	  // set up the directsound buffer



	  // allocate the memory to load sound data

	snd_buffer = (UCHAR *)malloc(child.cksize);



	// read the wave data

	mmioRead(hwav, (char *)snd_buffer, child.cksize);



	// close the file

	mmioClose(hwav, 0);



	// set rate and size in data structure

	sound_fx[sound_id].rate = wfmtx.nSamplesPerSec;

	sound_fx[sound_id].size = child.cksize;

	sound_fx[sound_id].state = 1; // sound loaded



								  // set up the format data structure

	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));



	pcmwf.wFormatTag = WAVE_FORMAT_PCM;  // pulse code modulation

	pcmwf.nChannels = 1;                // mono

	pcmwf.nSamplesPerSec = 11025;            // always this rate

	pcmwf.nBlockAlign = 1;

	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;

	pcmwf.wBitsPerSample = 8;

	pcmwf.cbSize = 0;



	// prepare to create sounds buffer

	dsbd.dwSize = sizeof(DSBUFFERDESC);

	dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;

	dsbd.dwBufferBytes = child.cksize;

	dsbd.lpwfxFormat = &pcmwf;



	// create the sound buffer

	if (FAILED(lpds->CreateSoundBuffer(&dsbd, &sound_fx[sound_id].dsbuffer, NULL)))

	{

		// release memory

		free(snd_buffer);



		// return error

		return(-1);

	} // end if



	  // copy data into sound buffer

	if (FAILED(sound_fx[sound_id].dsbuffer->Lock(0,

		child.cksize,

		(void **)&audio_ptr_1,

		&audio_length_1,

		(void **)&audio_ptr_2,

		&audio_length_2,

		DSBLOCK_FROMWRITECURSOR)))

		return(0);



	// copy first section of circular buffer

	memcpy(audio_ptr_1, snd_buffer, audio_length_1);



	// copy last section of circular buffer

	memcpy(audio_ptr_2, (snd_buffer + audio_length_1), audio_length_2);



	// unlock the buffer

	if (FAILED(sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1,

		audio_length_1,

		audio_ptr_2,

		audio_length_2)))

		return(0);



	// release the temp buffer

	free(snd_buffer);



	// return id

	return(sound_id);
}




//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	main_instance = hinstance;

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "DIDS";

	if( !RegisterClass(&wc) ) 
	{
		MessageBox(0, "RegisterClass() - FAILED", 0, 0);
		return false;
	}
		
	main_hwnd = CreateWindow("DIDS", "Direct Input with Direct Sound", WS_EX_TOPMOST, 0, 0, width, height,
						0 , 0 , hinstance, 0 ); 

	if( !main_hwnd )
	{
		MessageBox(0, "CreateWindow() - FAILED", 0, 0);
		return false;
	}

	ShowWindow(main_hwnd, SW_SHOW);

	
	if(!Setup())
	{
		MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}


	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime(); 

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)* 0.005f;

			Display(timeDelta);

			lastTime = currTime;
        }

	}

	Cleanup();

	Device->Release();

	return 0;
}
