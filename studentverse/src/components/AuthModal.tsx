'use client'

import { useState, type FormEvent } from 'react'
import { createClient } from '@/lib/supabase'
import { CUNY_CAMPUSES } from '@/lib/cuny'
import { X, Mail, School, User } from 'lucide-react'

interface AuthModalProps {
  isOpen: boolean
  onClose: () => void
  onSuccess: () => void
}

export default function AuthModal({ isOpen, onClose, onSuccess }: AuthModalProps) {
  const supabase = createClient()
  const [email, setEmail] = useState('')
  const [fullName, setFullName] = useState('')
  const [campus, setCampus] = useState(CUNY_CAMPUSES[0])
  const [loading, setLoading] = useState(false)
  const [message, setMessage] = useState<string | null>(null)

  if (!isOpen) return null

  async function handleAuth(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    setLoading(true)
    setMessage(null)

    // Send OTP Magic Link via Supabase Auth
    const { error } = await supabase.auth.signInWithOtp({
      email,
      options: {
        data: {
          full_name: fullName,
          campus: campus,
        },
      },
    })

    if (error) {
      alert(error.message)
      setLoading(false)
      return
    }

    setMessage('Check your email! We sent you a secure magic login link.')
    setLoading(false)
    setTimeout(() => {
      onSuccess()
    }, 4000)
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/70 backdrop-blur-sm p-4">
      <div className="bg-slate-900 border border-slate-800 rounded-2xl max-w-md w-full p-6 relative shadow-2xl">
        <button
          type="button"
          onClick={onClose}
          className="absolute top-4 right-4 text-slate-400 hover:text-white"
        >
          <X className="w-5 h-5" />
        </button>

        <div className="mb-6">
          <h2 className="text-xl font-bold text-white">Join CUNYVerse</h2>
          <p className="text-xs text-slate-400 mt-1">
            Connect with 25 CUNY campuses. Sign in with your student email.
          </p>
        </div>

        {message ? (
          <div className="bg-emerald-950/60 border border-emerald-800 text-emerald-300 p-4 rounded-xl text-sm">
            {message}
          </div>
        ) : (
          <form onSubmit={handleAuth} className="space-y-4">
            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1.5">
                Full Name
              </label>
              <div className="relative">
                <User className="w-4 h-4 text-slate-500 absolute left-3 top-3" />
                <input
                  type="text"
                  placeholder="e.g. Jane Doe"
                  value={fullName}
                  onChange={(e) => setFullName(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-lg pl-9 pr-3 py-2 text-sm text-slate-100 outline-none focus:border-blue-500"
                  required
                />
              </div>
            </div>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1.5">
                Home Campus
              </label>
              <div className="relative">
                <School className="w-4 h-4 text-slate-500 absolute left-3 top-3" />
                <select
                  value={campus}
                  onChange={(e) => setCampus(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-lg pl-9 pr-3 py-2 text-sm text-slate-100 outline-none focus:border-blue-500"
                >
                  {CUNY_CAMPUSES.map((c) => (
                    <option key={c} value={c}>
                      {c}
                    </option>
                  ))}
                </select>
              </div>
            </div>

            <div>
              <label className="block text-xs font-semibold text-slate-300 mb-1.5">
                Student Email
              </label>
              <div className="relative">
                <Mail className="w-4 h-4 text-slate-500 absolute left-3 top-3" />
                <input
                  type="email"
                  placeholder="jane.doe@baruchmail.cuny.edu"
                  value={email}
                  onChange={(e) => setEmail(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-lg pl-9 pr-3 py-2 text-sm text-slate-100 outline-none focus:border-blue-500"
                  required
                />
              </div>
            </div>

            <button
              type="submit"
              disabled={loading}
              className="w-full bg-blue-600 hover:bg-blue-700 disabled:opacity-50 text-white font-medium py-2 rounded-lg text-sm transition mt-2"
            >
              {loading ? 'Sending link...' : 'Send Magic Sign-In Link'}
            </button>
          </form>
        )}
      </div>
    </div>
  )
}