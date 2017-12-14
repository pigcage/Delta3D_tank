#include "stdafx.h"
#include <iostream>

#include <osgGA/GUIEventAdapter>

#include <dtAudio/audiomanager.h>
#include <dtABC/application.h>
#include <dtCore/object.h>
#include <dtCore/keyboard.h>
#include <dtCore/refptr.h>
#include <dtCore/transform.h>
#include <dtCore/walkmotionmodel.h>



using namespace dtABC;
using namespace dtCore;
using namespace dtAudio;
using namespace std;


class App : public Application{
public:
	App(){
		//Sound init
		AudioManager::Instantiate();
		AudioManager::GetInstance().LoadFile("../Image.wav");

		_sound=AudioManager::GetInstance().NewSound();
		_sound->LoadFile("../Image.wav");
	}

	~App(){
		AudioManager::GetInstance().FreeSound(_sound);
		AudioManager::Destroy();
	}

	//Keyboard onclick events
	virtual bool KeyPressed(const Keyboard* keyboard, int kc){

		cout<<"a key is pressed"<<k++<<endl;


		if(keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Up)||keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Down)||keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Left)||keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Right||kc==osgGA::GUIEventAdapter::KEY_Up||kc==osgGA::GUIEventAdapter::KEY_Down||kc==osgGA::GUIEventAdapter::KEY_Left||kc==osgGA::GUIEventAdapter::KEY_Right)){
			if(!_sound->IsPlaying() && model->IsEnabled()){
				_sound->Play();
			}
		}

		//Must start moving first
		if(keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Up)||keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Down)){

			model->SetEnabled(true);
			
			//Left Ctrl to accelerate, maximun speed 40.0f
			if(keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Control_L)){
				if(speed<30.0f){
					speed += 0.5f;
					SetWalkMode(brdm,speed);
				}
			}
			//Left Shift to decelerate, minimum speed 6.0f 
			if(keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_L)){
				if(speed>6.0f){
					speed -= 0.5f;
					SetWalkMode(brdm,speed);
				}
			}
			
			return true;
		}

		//Any other button is pressed
		else {
			return false;
		}
	}
	

	//Stop sound and return to normal speed when stop
	virtual bool KeyReleased(const Keyboard* keyboard,int kr){
		if(kr==osgGA::GUIEventAdapter::KEY_Up||kr==osgGA::GUIEventAdapter::KEY_Down){
			model->SetEnabled(false);
			if(_sound->IsPlaying()){
				_sound->Stop();
			}
			speed=6.0f;
			SetWalkMode(brdm,speed);
			cout<<"end"<<endl;
		}
		return true;
	}
	
	virtual bool MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button){
		if(!_sound->IsPlaying() && model->IsEnabled()){
			_sound->Play();
		}
		return true;
	};
	virtual bool MouseButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button){
		if(_sound->IsPlaying()){
			_sound->Stop();
		}
		return true;
	};

	virtual void Config(){

		Application::Config();
	
		//Add tank
		brdm = new Object("brdm");
		brdm->LoadFile("../module/brdm.ive");
		AddDrawable(brdm.get());

		speed = 6.0f;
		
		//Add map
		flatdirt = new Object("flatdirt");
		flatdirt->LoadFile("../module/flatdirt.ive");
		AddDrawable(flatdirt.get());
		dtCore::Transform trans_1;
		osg::Vec3 fd1Position(0.0f,-10.0f,0.0f);
		osg::Vec3 fd1Rotation(0.0f,0.0f,0.0f);
		trans_1.SetTranslation(fd1Position);
		flatdirt->SetTransform(trans_1);

		//Set tank position
		osg::Vec3 brdmPosition(0.0f,-25.0f,0.0f);
		osg::Vec3 brdmRotation(0.0f,0.0f,0.0f);
		dtCore::Transform trans;
		trans.SetTranslation(brdmPosition);
		trans.SetRotation(brdmRotation);
		brdm->SetTransform(trans);

		//Set camera
		dtCore::Transform camPos;
		osg::Vec3 canXYZ(0.0f,-100.0f,35.0f);
		osg::Vec3 lookAtXYZ(brdmPosition);
		osg::Vec3 upVec(0.0f,0.0f,1.0f);
		camPos.Set(canXYZ,lookAtXYZ,upVec);
		GetCamera()->SetTransform(camPos);

		//Basic walk mode
		model = new WalkMotionModel(GetKeyboard(),GetMouse());
		SetWalkMode(brdm,speed);
		model->SetEnabled(false);
		cout<<"Congif finished, walk speed="<<model->GetMaximumWalkSpeed()<<endl;
	} 
	
	
	//Set walk mode
	void SetWalkMode(RefPtr<Object> obj ,float _speed){
		model->SetTarget(obj);
		model->SetMaximumWalkSpeed(_speed);
	}

private:
	RefPtr<Object> flatdirt;
	RefPtr<Object> brdm;
	RefPtr<dtCore::WalkMotionModel> model;
	Sound* _sound;
	float speed;
};

int main()
{
	App app;
	app.Config();
	app.Run();
	cout<<"program end"<<endl;
	return 0;
	
}

